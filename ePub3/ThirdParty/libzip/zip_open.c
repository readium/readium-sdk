/*
  zip_open.c -- open zip archive by name
  Copyright (C) 1999-2015 Dieter Baron and Thomas Klausner

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


#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zipint.h"

typedef enum {
    EXISTS_ERROR = -1,
    EXISTS_NOT = 0,
    EXISTS_EMPTY,
    EXISTS_NONEMPTY,
} exists_t;
static zip_t *_zip_allocate_new(zip_source_t *src, unsigned int flags, zip_error_t *error);
static zip_int64_t _zip_checkcons(zip_t *za, zip_cdir_t *cdir, zip_error_t *error);
static zip_cdir_t *_zip_find_central_dir(zip_t *za, zip_uint64_t len);
static exists_t _zip_file_exists(zip_source_t *src, zip_error_t *error);
static int _zip_headercomp(const zip_dirent_t *, const zip_dirent_t *);
static unsigned char *_zip_memmem(const unsigned char *, size_t, const unsigned char *, size_t);
static zip_cdir_t *_zip_read_cdir(zip_t *za, zip_buffer_t *buffer, zip_uint64_t buf_offset, zip_error_t *error);
static zip_cdir_t *_zip_read_eocd(zip_buffer_t *buffer, zip_uint64_t buf_offset, unsigned int flags, zip_error_t *error);
static zip_cdir_t *_zip_read_eocd64(zip_source_t *src, zip_buffer_t *buffer, zip_uint64_t buf_offset, unsigned int flags, zip_error_t *error);


ZIP_EXTERN zip_t *
zip_open(const char *fn, int _flags, int *zep)
{
    zip_t *za;
    zip_source_t *src;
    struct zip_error error;

    zip_error_init(&error);
    if ((src = zip_source_file_create(fn, 0, -1, &error)) == NULL) {
	_zip_set_open_error(zep, &error, 0);
	zip_error_fini(&error);
	return NULL;
    }

    if ((za = zip_open_from_source(src, _flags, &error)) == NULL) {
	zip_source_free(src);
	_zip_set_open_error(zep, &error, 0);
	zip_error_fini(&error);
	return NULL;
    }

    zip_error_fini(&error);
    return za;
}


ZIP_EXTERN zip_t *
zip_open_from_source(zip_source_t *src, int _flags, zip_error_t *error)
{
    static zip_int64_t needed_support_read = -1;
    static zip_int64_t needed_support_write = -1;
    
    unsigned int flags;
    zip_int64_t supported;
    exists_t exists;
    
    if (_flags < 0 || src == NULL) {
	zip_error_set(error, ZIP_ER_INVAL, 0);
        return NULL;
    }
    flags = (unsigned int)_flags;
    
    supported = zip_source_supports(src);
    if (needed_support_read == -1) {
        needed_support_read = zip_source_make_command_bitmap(ZIP_SOURCE_OPEN, ZIP_SOURCE_READ, ZIP_SOURCE_CLOSE, ZIP_SOURCE_SEEK, ZIP_SOURCE_TELL, ZIP_SOURCE_STAT, -1);
        needed_support_write = zip_source_make_command_bitmap(ZIP_SOURCE_BEGIN_WRITE, ZIP_SOURCE_COMMIT_WRITE, ZIP_SOURCE_ROLLBACK_WRITE, ZIP_SOURCE_SEEK_WRITE, ZIP_SOURCE_TELL_WRITE, ZIP_SOURCE_REMOVE, -1);
    }
    if ((supported & needed_support_read) != needed_support_read) {
        zip_error_set(error, ZIP_ER_OPNOTSUPP, 0);
        return NULL;
    }
    if ((supported & needed_support_write) != needed_support_write) {
        flags |= ZIP_RDONLY;
    }

    if ((flags & (ZIP_RDONLY|ZIP_TRUNCATE)) == (ZIP_RDONLY|ZIP_TRUNCATE)) {
	zip_error_set(error, ZIP_ER_RDONLY, 0);
	return NULL;
    }

    exists = _zip_file_exists(src, error);
    switch (exists) { 
    case EXISTS_ERROR:
	return NULL;

    case EXISTS_NOT:
	if ((flags & ZIP_CREATE) == 0) {
	    zip_error_set(error, ZIP_ER_NOENT, 0);
	    return NULL;
	}
	return _zip_allocate_new(src, flags, error);

    default: {
	zip_t *za;
	if (flags & ZIP_EXCL) {
	    zip_error_set(error, ZIP_ER_EXISTS, 0);
	    return NULL;
	}
	if (zip_source_open(src) < 0) {
	    _zip_error_set_from_source(error, src);
	    return NULL;
	}

	if (flags & ZIP_TRUNCATE) {
	    za = _zip_allocate_new(src, flags, error);
	}
	else {
	    /* ZIP_CREATE gets ignored if file exists and not ZIP_EXCL, just like open() */
	    za = _zip_open(src, flags, error);
	}

	if (za == NULL) {
	    zip_source_close(src);
	    return NULL;
	}
	return za;
    }
    }
}

ZIP_EXTERN int
zip_archive_set_tempdir(zip_t *za, const char *tempdir)
{
    char *new_tempdir;
    
    if (tempdir) {
        if ((new_tempdir = strdup(tempdir)) == NULL) {
            zip_error_set(&za->error, ZIP_ER_MEMORY, errno);
            return -1;
        }
    }
    else
        new_tempdir = NULL;
    
    free(za->tempdir);
    za->tempdir = new_tempdir;
    
    return 0;
}

zip_t *
_zip_open(zip_source_t *src, unsigned int flags, zip_error_t *error)
{
    zip_t *za;
    zip_cdir_t *cdir;
    struct zip_stat st;
    zip_uint64_t len;

    zip_stat_init(&st);
    if (zip_source_stat(src, &st) < 0) {
	_zip_error_set_from_source(error, src);
	return NULL;
    }
    if ((st.valid & ZIP_STAT_SIZE) == 0) {
	zip_error_set(error, ZIP_ER_SEEK, EOPNOTSUPP);
	return NULL;
    }
    len = st.size;

    /* treat empty files as empty archives */
    if (len == 0) {
	if ((za=_zip_allocate_new(src, flags, error)) == NULL) {
	    zip_source_free(src);
	    return NULL;
	}

	return za;
    }

    if ((za=_zip_allocate_new(src, flags, error)) == NULL) {
        return NULL;
    }
    
    if ((cdir = _zip_find_central_dir(za, len)) == NULL) {
        _zip_error_copy(error, &za->error);
	/* keep src so discard does not get rid of it */
	zip_source_keep(src);
	zip_discard(za);
	return NULL;
    }

    za->entry = cdir->entry;
    za->nentry = cdir->nentry;
    za->nentry_alloc = cdir->nentry_alloc;
    za->comment_orig = cdir->comment;
    
    za->ch_flags = za->flags;

    free(cdir);

    return za;
}


void
_zip_set_open_error(int *zep, const zip_error_t *err, int ze)
{
    if (err) {
	ze = zip_error_code_zip(err);
	if (zip_error_system_type(err) == ZIP_ET_SYS) {
	    errno = zip_error_code_system(err);
	}
    }

    if (zep)
	*zep = ze;
}


/* _zip_readcdir:
   tries to find a valid end-of-central-directory at the beginning of
   buf, and then the corresponding central directory entries.
   Returns a struct zip_cdir which contains the central directory 
   entries, or NULL if unsuccessful. */

static zip_cdir_t *
_zip_read_cdir(zip_t *za, zip_buffer_t *buffer, zip_uint64_t buf_offset, zip_error_t *error)
{
    zip_cdir_t *cd;
    zip_uint16_t comment_len;
    zip_uint64_t i, left;
    zip_uint64_t eocd_offset = _zip_buffer_offset(buffer);
    zip_buffer_t *cd_buffer;

    if (_zip_buffer_left(buffer) < EOCDLEN) {
	/* not enough bytes left for comment */
	zip_error_set(error, ZIP_ER_NOZIP, 0);
	return NULL;
    }
    
    /* check for end-of-central-dir magic */
    if (memcmp(_zip_buffer_get(buffer, 4), EOCD_MAGIC, 4) != 0) {
	zip_error_set(error, ZIP_ER_NOZIP, 0);
	return NULL;
    }

    if (_zip_buffer_get_32(buffer) != 0) {
	zip_error_set(error, ZIP_ER_MULTIDISK, 0);
	return NULL;
    }

    if (eocd_offset >= EOCD64LOCLEN && memcmp(_zip_buffer_data(buffer) + eocd_offset - EOCD64LOCLEN, EOCD64LOC_MAGIC, 4) == 0) {
        _zip_buffer_set_offset(buffer, eocd_offset - EOCD64LOCLEN);
        cd = _zip_read_eocd64(za->src, buffer, buf_offset, za->flags, error);
    }
    else {
        _zip_buffer_set_offset(buffer, eocd_offset);
        cd = _zip_read_eocd(buffer, buf_offset, za->flags, error);
    }

    if (cd == NULL)
	return NULL;

    _zip_buffer_set_offset(buffer, eocd_offset + 20);
    comment_len = _zip_buffer_get_16(buffer);

    if (cd->offset + cd->size > buf_offset + eocd_offset) {
	/* cdir spans past EOCD record */
	zip_error_set(error, ZIP_ER_INCONS, 0);
	_zip_cdir_free(cd);
	return NULL;
    }

    if (comment_len || (za->open_flags & ZIP_CHECKCONS)) {
        zip_uint64_t tail_len;
        
        _zip_buffer_set_offset(buffer, eocd_offset + EOCDLEN);
        tail_len = _zip_buffer_left(buffer);
        
        if (tail_len < comment_len || ((za->open_flags & ZIP_CHECKCONS) && tail_len != comment_len)) {
            zip_error_set(error, ZIP_ER_INCONS, 0);
            _zip_cdir_free(cd);
            return NULL;
        }

        if (comment_len) {
            if ((cd->comment=_zip_string_new(_zip_buffer_get(buffer, comment_len), comment_len, ZIP_FL_ENC_GUESS, error)) == NULL) {
                _zip_cdir_free(cd);
                return NULL;
            }
        }
    }

    if (cd->offset >= buf_offset) {
        zip_uint8_t *data;
	/* if buffer already read in, use it */
        _zip_buffer_set_offset(buffer, cd->offset - buf_offset);
        
        if ((data = _zip_buffer_get(buffer, cd->size)) == NULL) {
            zip_error_set(error, ZIP_ER_INCONS, 0);
            _zip_cdir_free(cd);
            return NULL;
        }
        if ((cd_buffer = _zip_buffer_new(data, cd->size)) == NULL) {
            zip_error_set(error, ZIP_ER_MEMORY, 0);
            _zip_cdir_free(cd);
            return NULL;
        }
    }
    else {
        cd_buffer = NULL;
        
        if (zip_source_seek(za->src, (zip_int64_t)cd->offset, SEEK_SET) < 0) {
            _zip_error_set_from_source(error, za->src);
            _zip_cdir_free(cd);
            return NULL;
        }

	/* possible consistency check: cd->offset = len-(cd->size+cd->comment_len+EOCDLEN) ? */
	if (zip_source_tell(za->src) != (zip_int64_t)cd->offset) {
            zip_error_set(error, ZIP_ER_NOZIP, 0);
	    _zip_cdir_free(cd);
	    return NULL;
	}
    }

    left = (zip_uint64_t)cd->size;
    i=0;
    while (i<cd->nentry && left > 0) {
        zip_int64_t entry_size;
	if ((cd->entry[i].orig=_zip_dirent_new()) == NULL || (entry_size = _zip_dirent_read(cd->entry[i].orig, za->src, cd_buffer, false, error)) < 0) {
	    _zip_cdir_free(cd);
            _zip_buffer_free(cd_buffer);
	    return NULL;
	}
	i++;
        left -= (zip_uint64_t)entry_size;
    }
    
    if (i != cd->nentry) {
        zip_error_set(error, ZIP_ER_INCONS, 0);
        _zip_buffer_free(cd_buffer);
        _zip_cdir_free(cd);
        return NULL;
    }
    
    if (za->open_flags & ZIP_CHECKCONS) {
        bool ok;
        
        if (cd_buffer) {
            ok = _zip_buffer_eof(cd_buffer);
        }
        else {
            zip_int64_t offset = zip_source_tell(za->src);
            
            if (offset < 0) {
                _zip_error_set_from_source(error, za->src);
                _zip_buffer_free(cd_buffer);
                _zip_cdir_free(cd);
                return NULL;
            }
            ok = ((zip_uint64_t)offset == cd->offset + cd->size);
        }
        
        if (!ok) {
            zip_error_set(error, ZIP_ER_INCONS, 0);
            _zip_buffer_free(cd_buffer);
            _zip_cdir_free(cd);
            return NULL;
        }
    }

    _zip_buffer_free(cd_buffer);
    return cd;
}


/* _zip_checkcons:
   Checks the consistency of the central directory by comparing central
   directory entries with local headers and checking for plausible
   file and header offsets. Returns -1 if not plausible, else the
   difference between the lowest and the highest fileposition reached */

static zip_int64_t
_zip_checkcons(zip_t *za, zip_cdir_t *cd, zip_error_t *error)
{
    zip_uint64_t i;
    zip_uint64_t min, max, j;
    struct zip_dirent temp;

    _zip_dirent_init(&temp);
    if (cd->nentry) {
	max = cd->entry[0].orig->offset;
	min = cd->entry[0].orig->offset;
    }
    else
	min = max = 0;

    for (i=0; i<cd->nentry; i++) {
	if (cd->entry[i].orig->offset < min)
	    min = cd->entry[i].orig->offset;
	if (min > (zip_uint64_t)cd->offset) {
	    zip_error_set(error, ZIP_ER_NOZIP, 0);
	    return -1;
	}
	
	j = cd->entry[i].orig->offset + cd->entry[i].orig->comp_size
	    + _zip_string_length(cd->entry[i].orig->filename) + LENTRYSIZE;
	if (j > max)
	    max = j;
	if (max > (zip_uint64_t)cd->offset) {
	    zip_error_set(error, ZIP_ER_NOZIP, 0);
	    return -1;
	}
	
        if (zip_source_seek(za->src, (zip_int64_t)cd->entry[i].orig->offset, SEEK_SET) < 0) {
            _zip_error_set_from_source(error, za->src);
            return -1;
	}
	
	if (_zip_dirent_read(&temp, za->src, NULL, true, error) == -1) {
	    _zip_dirent_finalize(&temp);
	    return -1;
	}
	
	if (_zip_headercomp(cd->entry[i].orig, &temp) != 0) {
	    zip_error_set(error, ZIP_ER_INCONS, 0);
	    _zip_dirent_finalize(&temp);
	    return -1;
	}
	
	cd->entry[i].orig->extra_fields = _zip_ef_merge(cd->entry[i].orig->extra_fields, temp.extra_fields);
	cd->entry[i].orig->local_extra_fields_read = 1;
	temp.extra_fields = NULL;
	
	_zip_dirent_finalize(&temp);
    }

    return (max-min) < ZIP_INT64_MAX ? (zip_int64_t)(max-min) : ZIP_INT64_MAX;
}


/* _zip_headercomp:
   compares a central directory entry and a local file header
   Return 0 if they are consistent, -1 if not. */

static int
_zip_headercomp(const zip_dirent_t *central, const zip_dirent_t *local)
{
    if ((central->version_needed != local->version_needed)
#if 0
	/* some zip-files have different values in local
	   and global headers for the bitflags */
	|| (central->bitflags != local->bitflags)
#endif
	|| (central->comp_method != local->comp_method)
	|| (central->last_mod != local->last_mod)
	|| !_zip_string_equal(central->filename, local->filename))
	return -1;

    if ((central->crc != local->crc) || (central->comp_size != local->comp_size)
	|| (central->uncomp_size != local->uncomp_size)) {
	/* InfoZip stores valid values in local header even when data descriptor is used.
	   This is in violation of the appnote. */
	if (((local->bitflags & ZIP_GPBF_DATA_DESCRIPTOR) == 0
	     || local->crc != 0 || local->comp_size != 0 || local->uncomp_size != 0))
	    return -1;
    }

    return 0;
}


static zip_t *
_zip_allocate_new(zip_source_t *src, unsigned int flags, zip_error_t *error)
{
    zip_t *za;

    if ((za = _zip_new(error)) == NULL) {
	return NULL;
    }

    za->src = src;
    za->open_flags = flags;
    if (flags & ZIP_RDONLY) {
        za->flags |= ZIP_AFL_RDONLY;
        za->ch_flags |= ZIP_AFL_RDONLY;
    }
    return za;
}


/*
 * tests for file existence
 */
static exists_t
_zip_file_exists(zip_source_t *src, zip_error_t *error)
{
    struct zip_stat st;

    zip_stat_init(&st);
    if (zip_source_stat(src, &st) != 0) {
        zip_error_t *src_error = zip_source_error(src);
        if (zip_error_code_zip(src_error) == ZIP_ER_READ && zip_error_code_system(src_error) == ENOENT) {
	    return EXISTS_NOT;
	}
	_zip_error_copy(error, src_error);
	return EXISTS_ERROR;
    }

    return (st.valid & ZIP_STAT_SIZE) && st.size == 0 ? EXISTS_EMPTY : EXISTS_NONEMPTY;
}


static zip_cdir_t *
_zip_find_central_dir(zip_t *za, zip_uint64_t len)
{
    zip_cdir_t *cdir, *cdirnew;
    zip_uint8_t *match;
    zip_int64_t buf_offset;
    zip_uint64_t buflen;
    zip_int64_t a;
    zip_int64_t best;
    zip_error_t error;
    zip_buffer_t *buffer;

    if (len < EOCDLEN) {
	zip_error_set(&za->error, ZIP_ER_NOZIP, 0);
        return NULL;
    }

    buflen = (len < CDBUFSIZE ? len : CDBUFSIZE);
    if (zip_source_seek(za->src, -(zip_int64_t)buflen, SEEK_END) < 0) {
	zip_error_t *src_error = zip_source_error(za->src);
	if (zip_error_code_zip(src_error) != ZIP_ER_SEEK || zip_error_code_system(src_error) != EFBIG) {
	    /* seek before start of file on my machine */
	    _zip_error_copy(&za->error, src_error);
	    return NULL;
	}
    }
    if ((buf_offset = zip_source_tell(za->src)) < 0) {
        _zip_error_set_from_source(&za->error, za->src);
        return NULL;
    }
    
    if ((buffer = _zip_buffer_new_from_source(za->src, buflen, NULL, &za->error)) == NULL) {
        return NULL;
    }

    best = -1;
    cdir = NULL;
    if (buflen >= CDBUFSIZE) {
        /* EOCD64 locator is before EOCD, so leave place for it */
        _zip_buffer_set_offset(buffer, EOCD64LOCLEN);
    }
    zip_error_set(&error, ZIP_ER_NOZIP, 0);

    match = _zip_buffer_get(buffer, 0);
    while ((match=_zip_memmem(match, _zip_buffer_left(buffer)-(EOCDLEN-4), (const unsigned char *)EOCD_MAGIC, 4)) != NULL) {
        _zip_buffer_set_offset(buffer, (zip_uint64_t)(match - _zip_buffer_data(buffer)));
        if ((cdirnew = _zip_read_cdir(za, buffer, (zip_uint64_t)buf_offset, &error)) != NULL) {
            if (cdir) {
                if (best <= 0) {
                    best = _zip_checkcons(za, cdir, &error);
                }
                
                a = _zip_checkcons(za, cdirnew, &error);
                if (best < a) {
                    _zip_cdir_free(cdir);
                    cdir = cdirnew;
                    best = a;
                }
                else {
                    _zip_cdir_free(cdirnew);
                }
            }
            else {
                cdir = cdirnew;
                if (za->open_flags & ZIP_CHECKCONS)
                    best = _zip_checkcons(za, cdir, &error);
                else {
                    best = 0;
                }
            }
            cdirnew = NULL;
        }
        
        match++;
        _zip_buffer_set_offset(buffer, (zip_uint64_t)(match - _zip_buffer_data(buffer)));
    }

    _zip_buffer_free(buffer);
    
    if (best < 0) {
        _zip_error_copy(&za->error, &error);
        _zip_cdir_free(cdir);
        return NULL;
    }

    return cdir;
}


static unsigned char *
_zip_memmem(const unsigned char *big, size_t biglen, const unsigned char *little, size_t littlelen)
{
    const unsigned char *p;
    
    if ((biglen < littlelen) || (littlelen == 0))
	return NULL;
    p = big-1;
    while ((p=(const unsigned char *)
	        memchr(p+1, little[0], (size_t)(big-(p+1))+(size_t)(biglen-littlelen)+1)) != NULL) {
	if (memcmp(p+1, little+1, littlelen-1)==0)
	    return (unsigned char *)p;
    }

    return NULL;
}


static zip_cdir_t *_zip_read_eocd(zip_buffer_t *buffer, zip_uint64_t buf_offset, unsigned int flags, zip_error_t *error)
{
    zip_cdir_t *cd;
    zip_uint64_t i, nentry, size, offset, eocd_offset;

    if (_zip_buffer_left(buffer) < EOCDLEN) {
	zip_error_set(error, ZIP_ER_INCONS, 0);
	return NULL;
    }
    
    eocd_offset = _zip_buffer_offset(buffer);

    _zip_buffer_get(buffer, 8); /* magic and number of disks already verified */

    /* number of cdir-entries on this disk */
    i = _zip_buffer_get_16(buffer);
    /* number of cdir-entries */
    nentry = _zip_buffer_get_16(buffer);

    if (nentry != i) {
	zip_error_set(error, ZIP_ER_NOZIP, 0);
	return NULL;
    }

    size = _zip_buffer_get_32(buffer);
    offset = _zip_buffer_get_32(buffer);

    if (offset+size < offset) {
        zip_error_set(error, ZIP_ER_SEEK, EFBIG);
        return NULL;
    }
    
    if (offset+size > buf_offset + eocd_offset) {
	/* cdir spans past EOCD record */
	zip_error_set(error, ZIP_ER_INCONS, 0);
	return NULL;
    }

    if ((flags & ZIP_CHECKCONS) && offset+size != buf_offset + eocd_offset) {
	zip_error_set(error, ZIP_ER_INCONS, 0);
	return NULL;
    }

    if ((cd=_zip_cdir_new(nentry, error)) == NULL)
	return NULL;

    cd->size = size;
    cd->offset = offset;
    
    return cd;
}


static zip_cdir_t *
_zip_read_eocd64(zip_source_t *src, zip_buffer_t *buffer, zip_uint64_t buf_offset, unsigned int flags, zip_error_t *error)
{
    zip_cdir_t *cd;
    zip_uint64_t offset;
    zip_uint8_t eocd[EOCD64LEN];
    zip_uint64_t eocd_offset;
    zip_uint64_t size, nentry, i, eocdloc_offset;
    bool free_buffer;

    eocdloc_offset = _zip_buffer_offset(buffer);
    
    _zip_buffer_get(buffer, 8); /* magic and single disk already verified */
    eocd_offset = _zip_buffer_get_64(buffer);
    
    if (eocd_offset > ZIP_INT64_MAX || eocd_offset + EOCD64LEN < eocd_offset) {
        zip_error_set(error, ZIP_ER_SEEK, EFBIG);
        return NULL;
    }

    if (eocd_offset + EOCD64LEN > eocdloc_offset + buf_offset) {
	zip_error_set(error, ZIP_ER_INCONS, 0);
	return NULL;
    }

    if (eocd_offset >= buf_offset && eocd_offset + EOCD64LEN <= buf_offset + _zip_buffer_size(buffer)) {
        _zip_buffer_set_offset(buffer, eocd_offset - buf_offset);
        free_buffer = false;
    }
    else {
        if (zip_source_seek(src, (zip_int64_t)eocd_offset, SEEK_SET) < 0) {
            _zip_error_set_from_source(error, src);
            return NULL;
        }
        if ((buffer = _zip_buffer_new_from_source(src, EOCD64LEN, eocd, error)) == NULL) {
            return NULL;
        }
        free_buffer = true;
    }

    if (memcmp(_zip_buffer_get(buffer, 4), EOCD64_MAGIC, 4) != 0) {
	zip_error_set(error, ZIP_ER_INCONS, 0);
        if (free_buffer) {
            _zip_buffer_free(buffer);
        }
	return NULL;
    }
    
    size = _zip_buffer_get_64(buffer);

    if ((flags & ZIP_CHECKCONS) && size + eocd_offset + 12 != buf_offset + eocdloc_offset) {
	zip_error_set(error, ZIP_ER_INCONS, 0);
        if (free_buffer) {
            _zip_buffer_free(buffer);
        }
        return NULL;
    }

    _zip_buffer_get(buffer, 12); /* skip version made by/needed and num disks */
    
    nentry = _zip_buffer_get_64(buffer);
    i = _zip_buffer_get_64(buffer);

    if (nentry != i) {
	zip_error_set(error, ZIP_ER_MULTIDISK, 0);
        if (free_buffer) {
            _zip_buffer_free(buffer);
        }
	return NULL;
    }

    size = _zip_buffer_get_64(buffer);
    offset = _zip_buffer_get_64(buffer);
    
    if (!_zip_buffer_ok(buffer)) {
        zip_error_set(error, ZIP_ER_INTERNAL, 0);
        if (free_buffer) {
            _zip_buffer_free(buffer);
        }
        return NULL;
    }

    if (free_buffer) {
        _zip_buffer_free(buffer);
    }

    if (offset > ZIP_INT64_MAX || offset+size < offset) {
        zip_error_set(error, ZIP_ER_SEEK, EFBIG);
        return NULL;
    }
    if ((flags & ZIP_CHECKCONS) && offset+size != eocd_offset) {
	zip_error_set(error, ZIP_ER_INCONS, 0);
	return NULL;
    }

    if ((cd=_zip_cdir_new(nentry, error)) == NULL)
	return NULL;

    
    cd->size = size;
    cd->offset = offset;

    return cd;
}
