#ifndef _SHA1_H
#define _SHA1_H

#ifdef __cplusplus
extern "C" {
#endif

// The typedefs for 8-bit, 16-bit and 32-bit unsigned integers
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;

struct sha1_context
{
    uint32 total[2];
    uint32 state[5];
    uint8 buffer[64];
};

void sha1_starts( struct sha1_context *ctx );
void sha1_update( struct sha1_context *ctx, uint8 *input, uint32 length );
void sha1_finish( struct sha1_context *ctx, uint8 digest[20] );

#ifdef __cplusplus
}
#endif

#endif /* sha1.h */
