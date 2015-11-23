/*
  zip_fopen_index.c -- seek to a location within a file in a zip archive
  Copyright (C) 1999-2013 Dieter Baron and Thomas Klausner
  Original implementation contributed by Jim Dovey

  This file is part of libzip, a library to manipulate ZIP archives.
  The authors can be contacted at <libzip@nih.at>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
  3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.
 
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zipint.h"

#if defined(_MSC_VER)
# define strdup _strdup
# define fseeko fseek
# define ftello ftell
# define fileno _fileno
#endif

static int _zip_fseek_bytes(struct zip_file* zf, off_t abspos, off_t flen);
static int _zip_fseek_comp(struct zip_file* zf, off_t abspos, off_t flen);

/* helpers for dealing with inline decompression */
static int _zip_fseek_to_start(struct zip_file* zf);
static int _zip_fseek_by_reading(struct zip_file* zf, size_t toread);

ZIP_EXTERN int
zip_fseek(struct zip_file *zf, long pos, int whence)
{
    off_t abspos, flen;
    
    if (!zf)
        return -1;
    
    if (zf->error.zip_err != 0)
        return -1;
    
    if (pos == 0 && whence == ZIP_SEEK_CUR)
        return 0;
    
    flen = zf->za->cdir->entry[zf->file_index].uncomp_size;
    
    switch (whence)
    {
        case ZIP_SEEK_CUR:
            abspos = zf->file_fpos + pos;
            break;
        case ZIP_SEEK_SET:
            abspos = pos;
            break;
        case ZIP_SEEK_END:
            abspos = flen + pos;
            break;
        default:
            /* incorrect/unspecified 'whence' parameter */
            _zip_error_set(&zf->error, ZIP_ER_INVAL, 0);
            return -1;
    }
    
    if (abspos == zf->file_fpos)
        return 0;       /* no change */
    
    if ((zf->flags & ZIP_ZF_DECOMP) == 0) {
        // file data is not compressed, or file was opened to read compressed data directly
        return _zip_fseek_bytes(zf, abspos, flen);
    }
    else {
        return _zip_fseek_comp(zf, abspos, flen);
    }
}

/* seeking by raw byte amounts - no compression/decompression to handle */
int _zip_fseek_bytes(struct zip_file* zf, off_t abspos, off_t flen)
{
    /* can't set a negative offset */
    if (abspos < 0) {
        _zip_error_set(&zf->error, ZIP_ER_INVAL, 0);
        return -1;
    }
    /* CAN set offset past EOF: keeps offset, sets EOF */
    else if (abspos >= flen) {
        zf->flags |= ZIP_ZF_EOF;
        zf->bytes_left = 0;
        
        /* added by DRM inside, C.H. Yu on 2015-04-13 */
        // Without following added codes, uncompressed EPUB media content would not be properly random accessed,
        // such as bad index access error
        zf->fpos = _zip_file_get_offset_safe(zf->za, zf->file_index);
        zf->fpos += flen;
        zf->cbytes_left = 0;
        /* adding end */
    }
    /* not at or past EOF? ensure EOF is unset and update bytes_left */
    else {
        zf->flags &= ~ZIP_ZF_EOF;
        zf->bytes_left = flen - abspos;
        
        /* added by DRM inside, C.H. Yu on 2015-04-13 */
        // Without following added codes, uncompressed EPUB media content would not be properly random accessed,
        // such as bad index access error
        zf->fpos += (abspos - zf->file_fpos);
        zf->cbytes_left = zf->bytes_left;
        /* adding end */
    }
    zf->file_fpos = abspos;
    return 0;
}

/* seeking by raw byte amounts - no compression/decompression to handle */
int _zip_fseek_comp(struct zip_file* zf, off_t abspos, off_t flen)
{
    if (abspos >= flen) {
        // simple case -- set EOF
        zf->flags |= ZIP_ZF_EOF;
        zf->bytes_left = zf->cbytes_left = 0;
        zf->file_fpos = abspos;
        return 0;
    }
    else if (abspos > zf->file_fpos) {
        // read & decompress bytes until we reach the right position
        return _zip_fseek_by_reading(zf, abspos-zf->file_fpos);
    }
    
    /* at this point, we're definitely moving backwards */
    
    /* can't set a negative offset */
    if (abspos < 0) {
        _zip_error_set(&zf->error, ZIP_ER_INVAL, 0);
        return -1;
    }
    
    if (_zip_fseek_to_start(zf) < 0)
        return -1;      /* error already set */
    
    /* this is a no-op for abspos == 0 */
    return _zip_fseek_by_reading(zf, abspos);
}

int _zip_fseek_to_start(struct zip_file* zf)
{
    int len, ret;
    
    zf->flags &= ~ZIP_ZF_EOF;
    zf->file_fpos = 0;
    zf->bytes_left = zf->za->cdir->entry[zf->file_index].uncomp_size;
    zf->cbytes_left = zf->za->cdir->entry[zf->file_index].comp_size;
    zf->fpos = _zip_file_get_offset_safe(zf->za, zf->file_index);
    
    len = _zip_file_fillbuf(zf->buffer, BUFSIZE, zf);
    
    zf->zstr->zalloc = Z_NULL;
    zf->zstr->zfree = Z_NULL;
	zf->zstr->opaque = NULL;
	zf->zstr->next_in = (Bytef *)zf->buffer;
	zf->zstr->avail_in = len;
    
    /* negative value to tell zlib that there is no header */
    if ((ret=inflateInit2(zf->zstr, -MAX_WBITS)) != Z_OK) {
        _zip_error_set(&zf->error, ZIP_ER_ZLIB, ret);
        return -1;
    }
    
    return 0;
}

int _zip_fseek_by_reading(struct zip_file* zf, size_t toread)
{
    char bytes[1024];
    while (toread > 0) {
        ssize_t numRead = zip_fread(zf, bytes, (toread < 1024 ? toread : 1024));
        if (numRead < 0 )
            return -1;      /* error already set */
        if (numRead == 0) {
            /* avoid infinite loops */
            _zip_error_set(&zf->error, ZIP_ER_INCONS, 0);
            return -1;
        }
        
        toread -= numRead;
    }
    
    /* zf has been updated for us by zip_fread() already */
    return 0;
}
