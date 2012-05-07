/*
 *  NIST SP800-38D compliant GCM implementation
 *
 *  Copyright (C) 2006-2012, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*
 *  http://csrc.nist.gov/publications/nistpubs/800-38D/SP-800-38D.pdf
 */
#include "polarssl/config.h"

#if defined(POLARSSL_GCM_C)

#include "polarssl/gcm.h"

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_ULONG_BE
#define GET_ULONG_BE(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ] << 24 )        \
        | ( (unsigned long) (b)[(i) + 1] << 16 )        \
        | ( (unsigned long) (b)[(i) + 2] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 3]       );       \
}
#endif

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

static void gcm_gen_table( gcm_context *ctx )
{
    int i, j;
    uint64_t hi, lo;
    uint64_t vl, vh;
    unsigned char h[16];
    
    memset( h, 0, 16 );
    aes_crypt_ecb( &ctx->aes_ctx, AES_ENCRYPT, h, h );

    ctx->HH[0] = 0;
    ctx->HL[0] = 0;

    GET_ULONG_BE( hi, h,  0  );
    GET_ULONG_BE( lo, h,  4  );
    vh = (uint64_t) hi << 32 | lo;

    GET_ULONG_BE( hi, h,  8  );
    GET_ULONG_BE( lo, h,  12 );
    vl = (uint64_t) hi << 32 | lo;
    
    ctx->HL[8] = vl;
    ctx->HH[8] = vh;

    for( i = 4; i > 0; i >>= 1 )
    {
        uint32_t T = ( vl & 1 ) ? 0xe1000000U : 0;
        vl  = ( vh << 63 ) | ( vl >> 1 );
        vh  = ( vh >> 1 ) ^ ( (uint64_t) T << 32);

        ctx->HL[i] = vl;
        ctx->HH[i] = vh;
    }

    for (i = 2; i < 16; i <<= 1 )
    {
        uint64_t *HiL = ctx->HL + i, *HiH = ctx->HH + i;
        vh = *HiH;
        vl = *HiL;
        for( j = 1; j < i; j++ )
        {
            HiH[j] = vh ^ ctx->HH[j];
            HiL[j] = vl ^ ctx->HL[j];
        }
    }
    
}

int gcm_init( gcm_context *ctx, const unsigned char *key, unsigned int keysize )
{
    int ret;

    memset( ctx, 0, sizeof(gcm_context) );

    if( ( ret = aes_setkey_enc( &ctx->aes_ctx, key, keysize ) ) != 0 )
        return( ret );

    gcm_gen_table( ctx );

    return( 0 );
}

static const uint64_t last4[16] =
{
    0x0000, 0x1c20, 0x3840, 0x2460,
    0x7080, 0x6ca0, 0x48c0, 0x54e0,
    0xe100, 0xfd20, 0xd940, 0xc560,
    0x9180, 0x8da0, 0xa9c0, 0xb5e0
};

void gcm_mult( gcm_context *ctx, const unsigned char x[16], unsigned char output[16] )
{
    int i = 0;
    unsigned char z[16];
    unsigned char v[16];
    unsigned char lo, hi, rem;
    uint64_t zh, zl;

    memset( z, 0x00, 16 );
    memcpy( v, x, 16 );

    lo = x[15] & 0xf;
    hi = x[15] >> 4;

    zh = ctx->HH[lo];
    zl = ctx->HL[lo];

    for( i = 15; i >= 0; i-- )
    {
        lo = x[i] & 0xf;
        hi = x[i] >> 4;

        if( i != 15 )
        {
            rem = zl & 0xf;
            zl = ( zh << 60 ) | ( zl >> 4 );
            zh = ( zh >> 4 );
            zh ^= (uint64_t) last4[rem] << 48;
            zh ^= ctx->HH[lo];
            zl ^= ctx->HL[lo];

        }

        rem = zl & 0xf;
        zl = ( zh << 60 ) | ( zl >> 4 );
        zh = ( zh >> 4 );
        zh ^= (uint64_t) last4[rem] << 48;
        zh ^= ctx->HH[hi];
        zl ^= ctx->HL[hi];
    }

    PUT_ULONG_BE( zh >> 32, output, 0 );
    PUT_ULONG_BE( zh, output, 4 );
    PUT_ULONG_BE( zl >> 32, output, 8 );
    PUT_ULONG_BE( zl, output, 12 );
}

int gcm_crypt_and_tag( gcm_context *ctx,
                       int mode,
                       size_t length,
                       const unsigned char *iv,
                       size_t iv_len,
                       const unsigned char *add,
                       size_t add_len,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t tag_len,
                       unsigned char *tag )
{
    unsigned char y[16];
    unsigned char ectr[16];
    unsigned char buf[16];
    unsigned char work_buf[16];
    size_t i;
    unsigned char cb;
    const unsigned char *p;
    unsigned char *out_p = output;
    size_t use_len;
    size_t orig_len = length * 8;
    size_t orig_add_len = add_len * 8;
    unsigned char **xor_p;

    memset( y, 0x00, 16 );
    memset( work_buf, 0x00, 16 );
    memset( tag, 0x00, tag_len );
    memset( buf, 0x00, 16 );

    if( mode == GCM_ENCRYPT )
        xor_p = (unsigned char **) &out_p;
    else
        xor_p = (unsigned char **) &p;

    if( iv_len == 12 )
    {
        memcpy( y, iv, iv_len );
        y[15] = 1;
    }
    else
    {
        memset( work_buf, 0x00, 16 );
        PUT_ULONG_BE( iv_len * 8, work_buf, 12 );

        p = iv;
        while( iv_len > 0 )
        {
            use_len = ( iv_len < 16 ) ? iv_len : 16;

            if( use_len == 16 )
            {
                ((uint64_t *) y)[0] ^= ((uint64_t *) p)[0];
                ((uint64_t *) y)[1] ^= ((uint64_t *) p)[1];
            }
            else
                for( i = 0; i < use_len; i++ )
                    y[i] ^= p[i];
        
            gcm_mult( ctx, y, y );

            iv_len -= use_len;
            p += use_len;
        }

        ((uint64_t *) y)[0] ^= ((uint64_t *) work_buf)[0];
        ((uint64_t *) y)[1] ^= ((uint64_t *) work_buf)[1];

        gcm_mult( ctx, y, y );
    }

    aes_crypt_ecb( &ctx->aes_ctx, AES_ENCRYPT, y, ectr );
    memcpy( tag, ectr, tag_len );

    p = add;
    while( add_len > 0 )
    {
        use_len = ( add_len < 16 ) ? add_len : 16;

        if( use_len == 16 )
        {
            ((uint64_t *) buf)[0] ^= ((uint64_t *) p)[0];
            ((uint64_t *) buf)[1] ^= ((uint64_t *) p)[1];
        }
        else
            for( i = 0; i < use_len; i++ )
                buf[i] ^= p[i];
        
        gcm_mult( ctx, buf, buf );

        add_len -= use_len;
        p += use_len;
    }

    p = input;
    while( length > 0 )
    {
        use_len = ( length < 16 ) ? length : 16;

        i = 15;
        do {
            y[i]++;
            cb = y[i] == 0;
        } while( i-- && cb );

        aes_crypt_ecb( &ctx->aes_ctx, AES_ENCRYPT, y, ectr );

        if( use_len == 16 )
        {
            ((uint64_t *) out_p)[0] = ((uint64_t *) ectr)[0] ^
                                      ((uint64_t *) p)[0];
            ((uint64_t *) out_p)[1] = ((uint64_t *) ectr)[1] ^
                                      ((uint64_t *) p)[1];
        }
        else
            for( i = 0; i < use_len; i++ )
                out_p[i] = ectr[i] ^ p[i];

        if( use_len == 16 )
        {
            ((uint64_t *) buf)[0] ^= ((uint64_t *) (*xor_p))[0];
            ((uint64_t *) buf)[1] ^= ((uint64_t *) (*xor_p))[1];
        }
        else
            for( i = 0; i < use_len; i++ )
                buf[i] ^= (*xor_p)[i];
        
        gcm_mult( ctx, buf, buf );
        
        length -= use_len;
        p += use_len;
        out_p += use_len;
    }

    if( orig_len || orig_add_len )
    {
        memset( work_buf, 0x00, 16 );

        PUT_ULONG_BE( orig_add_len , work_buf, 4 );
        PUT_ULONG_BE( orig_len , work_buf, 12 );

        ((uint64_t *) buf)[0] ^= ((uint64_t *) work_buf)[0];
        ((uint64_t *) buf)[1] ^= ((uint64_t *) work_buf)[1];

        gcm_mult( ctx, buf, buf );

        if( tag_len == 16 )
        {
            ((uint64_t *) tag)[0] ^= ((uint64_t *) buf)[0];
            ((uint64_t *) tag)[1] ^= ((uint64_t *) buf)[1];
        }
        else
            for( i = 0; i < tag_len; i++ )
                tag[i] ^= buf[i];
    }

    return( 0 );
}

int gcm_auth_decrypt( gcm_context *ctx,
                      size_t length,
                      const unsigned char *iv,
                      size_t iv_len,
                      const unsigned char *add,
                      size_t add_len,
                      const unsigned char *tag, 
                      size_t tag_len,
                      const unsigned char *input,
                      unsigned char *output )
{
    unsigned char check_tag[16];

    gcm_crypt_and_tag( ctx, GCM_DECRYPT, length, iv, iv_len, add, add_len, input, output, tag_len, check_tag );

    if( memcmp( check_tag, tag, tag_len ) == 0 )
        return( 0 );

    memset( output, 0, length );

    return( POLARSSL_ERR_GCM_AUTH_FAILED );
}

#if defined(POLARSSL_SELF_TEST)

#include <stdio.h>

/*
 * GCM test vectors from:
 *
 * http://csrc.nist.gov/groups/STM/cavp/documents/mac/gcmtestvectors.zip
 */
#define MAX_TESTS   6

int key_index[MAX_TESTS] =
    { 0, 0, 1, 1, 1, 1 };

unsigned char key[MAX_TESTS][32] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
      0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,
      0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
      0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08 },  
};

size_t iv_len[MAX_TESTS] =
    { 12, 12, 12, 12, 8, 60 };

int iv_index[MAX_TESTS] =
    { 0, 0, 1, 1, 1, 2 };

unsigned char iv[MAX_TESTS][64] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00 },
    { 0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad,
      0xde, 0xca, 0xf8, 0x88 },
    { 0x93, 0x13, 0x22, 0x5d, 0xf8, 0x84, 0x06, 0xe5,
      0x55, 0x90, 0x9c, 0x5a, 0xff, 0x52, 0x69, 0xaa, 
      0x6a, 0x7a, 0x95, 0x38, 0x53, 0x4f, 0x7d, 0xa1,
      0xe4, 0xc3, 0x03, 0xd2, 0xa3, 0x18, 0xa7, 0x28, 
      0xc3, 0xc0, 0xc9, 0x51, 0x56, 0x80, 0x95, 0x39,
      0xfc, 0xf0, 0xe2, 0x42, 0x9a, 0x6b, 0x52, 0x54, 
      0x16, 0xae, 0xdb, 0xf5, 0xa0, 0xde, 0x6a, 0x57,
      0xa6, 0x37, 0xb3, 0x9b }, 
};

size_t add_len[MAX_TESTS] =
    { 0, 0, 0, 20, 20, 20 };

int add_index[MAX_TESTS] =
    { 0, 0, 0, 1, 1, 1 };

unsigned char additional[MAX_TESTS][64] =
{
    { 0x00 },
    { 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
      0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 
      0xab, 0xad, 0xda, 0xd2 },
};

size_t pt_len[MAX_TESTS] =
    { 0, 16, 64, 60, 60, 60 };

int pt_index[MAX_TESTS] =
    { 0, 0, 1, 1, 1, 1 };

unsigned char pt[MAX_TESTS][64] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5,
      0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
      0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
      0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
      0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
      0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
      0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
      0xba, 0x63, 0x7b, 0x39, 0x1a, 0xaf, 0xd2, 0x55 },
};

unsigned char ct[MAX_TESTS * 3][64] =
{
    { 0x00 },
    { 0x03, 0x88, 0xda, 0xce, 0x60, 0xb6, 0xa3, 0x92,
      0xf3, 0x28, 0xc2, 0xb9, 0x71, 0xb2, 0xfe, 0x78 },
    { 0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24,
      0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c, 
      0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0,
      0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e, 
      0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c,
      0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05, 
      0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97,
      0x3d, 0x58, 0xe0, 0x91, 0x47, 0x3f, 0x59, 0x85 },
    { 0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24,
      0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c, 
      0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0,
      0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e, 
      0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c,
      0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05, 
      0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97,
      0x3d, 0x58, 0xe0, 0x91 },
    { 0x61, 0x35, 0x3b, 0x4c, 0x28, 0x06, 0x93, 0x4a,
      0x77, 0x7f, 0xf5, 0x1f, 0xa2, 0x2a, 0x47, 0x55, 
      0x69, 0x9b, 0x2a, 0x71, 0x4f, 0xcd, 0xc6, 0xf8,
      0x37, 0x66, 0xe5, 0xf9, 0x7b, 0x6c, 0x74, 0x23, 
      0x73, 0x80, 0x69, 0x00, 0xe4, 0x9f, 0x24, 0xb2,
      0x2b, 0x09, 0x75, 0x44, 0xd4, 0x89, 0x6b, 0x42, 
      0x49, 0x89, 0xb5, 0xe1, 0xeb, 0xac, 0x0f, 0x07,
      0xc2, 0x3f, 0x45, 0x98 },
    { 0x8c, 0xe2, 0x49, 0x98, 0x62, 0x56, 0x15, 0xb6,
      0x03, 0xa0, 0x33, 0xac, 0xa1, 0x3f, 0xb8, 0x94, 
      0xbe, 0x91, 0x12, 0xa5, 0xc3, 0xa2, 0x11, 0xa8,
      0xba, 0x26, 0x2a, 0x3c, 0xca, 0x7e, 0x2c, 0xa7, 
      0x01, 0xe4, 0xa9, 0xa4, 0xfb, 0xa4, 0x3c, 0x90,
      0xcc, 0xdc, 0xb2, 0x81, 0xd4, 0x8c, 0x7c, 0x6f, 
      0xd6, 0x28, 0x75, 0xd2, 0xac, 0xa4, 0x17, 0x03,
      0x4c, 0x34, 0xae, 0xe5 },
    { 0x00 },
    { 0x98, 0xe7, 0x24, 0x7c, 0x07, 0xf0, 0xfe, 0x41,
      0x1c, 0x26, 0x7e, 0x43, 0x84, 0xb0, 0xf6, 0x00 }, 
    { 0x39, 0x80, 0xca, 0x0b, 0x3c, 0x00, 0xe8, 0x41,
      0xeb, 0x06, 0xfa, 0xc4, 0x87, 0x2a, 0x27, 0x57, 
      0x85, 0x9e, 0x1c, 0xea, 0xa6, 0xef, 0xd9, 0x84,
      0x62, 0x85, 0x93, 0xb4, 0x0c, 0xa1, 0xe1, 0x9c, 
      0x7d, 0x77, 0x3d, 0x00, 0xc1, 0x44, 0xc5, 0x25,
      0xac, 0x61, 0x9d, 0x18, 0xc8, 0x4a, 0x3f, 0x47, 
      0x18, 0xe2, 0x44, 0x8b, 0x2f, 0xe3, 0x24, 0xd9,
      0xcc, 0xda, 0x27, 0x10, 0xac, 0xad, 0xe2, 0x56 },
    { 0x39, 0x80, 0xca, 0x0b, 0x3c, 0x00, 0xe8, 0x41,
      0xeb, 0x06, 0xfa, 0xc4, 0x87, 0x2a, 0x27, 0x57, 
      0x85, 0x9e, 0x1c, 0xea, 0xa6, 0xef, 0xd9, 0x84,
      0x62, 0x85, 0x93, 0xb4, 0x0c, 0xa1, 0xe1, 0x9c, 
      0x7d, 0x77, 0x3d, 0x00, 0xc1, 0x44, 0xc5, 0x25, 
      0xac, 0x61, 0x9d, 0x18, 0xc8, 0x4a, 0x3f, 0x47, 
      0x18, 0xe2, 0x44, 0x8b, 0x2f, 0xe3, 0x24, 0xd9,
      0xcc, 0xda, 0x27, 0x10 }, 
    { 0x0f, 0x10, 0xf5, 0x99, 0xae, 0x14, 0xa1, 0x54,
      0xed, 0x24, 0xb3, 0x6e, 0x25, 0x32, 0x4d, 0xb8, 
      0xc5, 0x66, 0x63, 0x2e, 0xf2, 0xbb, 0xb3, 0x4f,
      0x83, 0x47, 0x28, 0x0f, 0xc4, 0x50, 0x70, 0x57, 
      0xfd, 0xdc, 0x29, 0xdf, 0x9a, 0x47, 0x1f, 0x75,
      0xc6, 0x65, 0x41, 0xd4, 0xd4, 0xda, 0xd1, 0xc9, 
      0xe9, 0x3a, 0x19, 0xa5, 0x8e, 0x8b, 0x47, 0x3f,
      0xa0, 0xf0, 0x62, 0xf7 }, 
    { 0xd2, 0x7e, 0x88, 0x68, 0x1c, 0xe3, 0x24, 0x3c,
      0x48, 0x30, 0x16, 0x5a, 0x8f, 0xdc, 0xf9, 0xff, 
      0x1d, 0xe9, 0xa1, 0xd8, 0xe6, 0xb4, 0x47, 0xef,
      0x6e, 0xf7, 0xb7, 0x98, 0x28, 0x66, 0x6e, 0x45, 
      0x81, 0xe7, 0x90, 0x12, 0xaf, 0x34, 0xdd, 0xd9,
      0xe2, 0xf0, 0x37, 0x58, 0x9b, 0x29, 0x2d, 0xb3, 
      0xe6, 0x7c, 0x03, 0x67, 0x45, 0xfa, 0x22, 0xe7,
      0xe9, 0xb7, 0x37, 0x3b }, 
    { 0x00 },
    { 0xce, 0xa7, 0x40, 0x3d, 0x4d, 0x60, 0x6b, 0x6e, 
      0x07, 0x4e, 0xc5, 0xd3, 0xba, 0xf3, 0x9d, 0x18 }, 
    { 0x52, 0x2d, 0xc1, 0xf0, 0x99, 0x56, 0x7d, 0x07, 
      0xf4, 0x7f, 0x37, 0xa3, 0x2a, 0x84, 0x42, 0x7d, 
      0x64, 0x3a, 0x8c, 0xdc, 0xbf, 0xe5, 0xc0, 0xc9, 
      0x75, 0x98, 0xa2, 0xbd, 0x25, 0x55, 0xd1, 0xaa, 
      0x8c, 0xb0, 0x8e, 0x48, 0x59, 0x0d, 0xbb, 0x3d, 
      0xa7, 0xb0, 0x8b, 0x10, 0x56, 0x82, 0x88, 0x38, 
      0xc5, 0xf6, 0x1e, 0x63, 0x93, 0xba, 0x7a, 0x0a, 
      0xbc, 0xc9, 0xf6, 0x62, 0x89, 0x80, 0x15, 0xad }, 
    { 0x52, 0x2d, 0xc1, 0xf0, 0x99, 0x56, 0x7d, 0x07, 
      0xf4, 0x7f, 0x37, 0xa3, 0x2a, 0x84, 0x42, 0x7d,  
      0x64, 0x3a, 0x8c, 0xdc, 0xbf, 0xe5, 0xc0, 0xc9, 
      0x75, 0x98, 0xa2, 0xbd, 0x25, 0x55, 0xd1, 0xaa, 
      0x8c, 0xb0, 0x8e, 0x48, 0x59, 0x0d, 0xbb, 0x3d, 
      0xa7, 0xb0, 0x8b, 0x10, 0x56, 0x82, 0x88, 0x38, 
      0xc5, 0xf6, 0x1e, 0x63, 0x93, 0xba, 0x7a, 0x0a, 
      0xbc, 0xc9, 0xf6, 0x62 }, 
    { 0xc3, 0x76, 0x2d, 0xf1, 0xca, 0x78, 0x7d, 0x32,
      0xae, 0x47, 0xc1, 0x3b, 0xf1, 0x98, 0x44, 0xcb, 
      0xaf, 0x1a, 0xe1, 0x4d, 0x0b, 0x97, 0x6a, 0xfa,
      0xc5, 0x2f, 0xf7, 0xd7, 0x9b, 0xba, 0x9d, 0xe0, 
      0xfe, 0xb5, 0x82, 0xd3, 0x39, 0x34, 0xa4, 0xf0,
      0x95, 0x4c, 0xc2, 0x36, 0x3b, 0xc7, 0x3f, 0x78, 
      0x62, 0xac, 0x43, 0x0e, 0x64, 0xab, 0xe4, 0x99,
      0xf4, 0x7c, 0x9b, 0x1f }, 
    { 0x5a, 0x8d, 0xef, 0x2f, 0x0c, 0x9e, 0x53, 0xf1,
      0xf7, 0x5d, 0x78, 0x53, 0x65, 0x9e, 0x2a, 0x20, 
      0xee, 0xb2, 0xb2, 0x2a, 0xaf, 0xde, 0x64, 0x19,
      0xa0, 0x58, 0xab, 0x4f, 0x6f, 0x74, 0x6b, 0xf4, 
      0x0f, 0xc0, 0xc3, 0xb7, 0x80, 0xf2, 0x44, 0x45,
      0x2d, 0xa3, 0xeb, 0xf1, 0xc5, 0xd8, 0x2c, 0xde, 
      0xa2, 0x41, 0x89, 0x97, 0x20, 0x0e, 0xf8, 0x2e,
      0x44, 0xae, 0x7e, 0x3f }, 
};

unsigned char tag[MAX_TESTS * 3][16] =
{
    { 0x58, 0xe2, 0xfc, 0xce, 0xfa, 0x7e, 0x30, 0x61,
      0x36, 0x7f, 0x1d, 0x57, 0xa4, 0xe7, 0x45, 0x5a },
    { 0xab, 0x6e, 0x47, 0xd4, 0x2c, 0xec, 0x13, 0xbd,
      0xf5, 0x3a, 0x67, 0xb2, 0x12, 0x57, 0xbd, 0xdf },
    { 0x4d, 0x5c, 0x2a, 0xf3, 0x27, 0xcd, 0x64, 0xa6,
      0x2c, 0xf3, 0x5a, 0xbd, 0x2b, 0xa6, 0xfa, 0xb4 }, 
    { 0x5b, 0xc9, 0x4f, 0xbc, 0x32, 0x21, 0xa5, 0xdb,
      0x94, 0xfa, 0xe9, 0x5a, 0xe7, 0x12, 0x1a, 0x47 },
    { 0x36, 0x12, 0xd2, 0xe7, 0x9e, 0x3b, 0x07, 0x85,
      0x56, 0x1b, 0xe1, 0x4a, 0xac, 0xa2, 0xfc, 0xcb },
    { 0x61, 0x9c, 0xc5, 0xae, 0xff, 0xfe, 0x0b, 0xfa,
      0x46, 0x2a, 0xf4, 0x3c, 0x16, 0x99, 0xd0, 0x50 },
    { 0xcd, 0x33, 0xb2, 0x8a, 0xc7, 0x73, 0xf7, 0x4b,
      0xa0, 0x0e, 0xd1, 0xf3, 0x12, 0x57, 0x24, 0x35 },
    { 0x2f, 0xf5, 0x8d, 0x80, 0x03, 0x39, 0x27, 0xab,
      0x8e, 0xf4, 0xd4, 0x58, 0x75, 0x14, 0xf0, 0xfb }, 
    { 0x99, 0x24, 0xa7, 0xc8, 0x58, 0x73, 0x36, 0xbf,
      0xb1, 0x18, 0x02, 0x4d, 0xb8, 0x67, 0x4a, 0x14 },
    { 0x25, 0x19, 0x49, 0x8e, 0x80, 0xf1, 0x47, 0x8f,
      0x37, 0xba, 0x55, 0xbd, 0x6d, 0x27, 0x61, 0x8c }, 
    { 0x65, 0xdc, 0xc5, 0x7f, 0xcf, 0x62, 0x3a, 0x24,
      0x09, 0x4f, 0xcc, 0xa4, 0x0d, 0x35, 0x33, 0xf8 }, 
    { 0xdc, 0xf5, 0x66, 0xff, 0x29, 0x1c, 0x25, 0xbb,
      0xb8, 0x56, 0x8f, 0xc3, 0xd3, 0x76, 0xa6, 0xd9 }, 
    { 0x53, 0x0f, 0x8a, 0xfb, 0xc7, 0x45, 0x36, 0xb9,
      0xa9, 0x63, 0xb4, 0xf1, 0xc4, 0xcb, 0x73, 0x8b }, 
    { 0xd0, 0xd1, 0xc8, 0xa7, 0x99, 0x99, 0x6b, 0xf0,
      0x26, 0x5b, 0x98, 0xb5, 0xd4, 0x8a, 0xb9, 0x19 }, 
    { 0xb0, 0x94, 0xda, 0xc5, 0xd9, 0x34, 0x71, 0xbd,
      0xec, 0x1a, 0x50, 0x22, 0x70, 0xe3, 0xcc, 0x6c }, 
    { 0x76, 0xfc, 0x6e, 0xce, 0x0f, 0x4e, 0x17, 0x68,
      0xcd, 0xdf, 0x88, 0x53, 0xbb, 0x2d, 0x55, 0x1b }, 
    { 0x3a, 0x33, 0x7d, 0xbf, 0x46, 0xa7, 0x92, 0xc4,
      0x5e, 0x45, 0x49, 0x13, 0xfe, 0x2e, 0xa8, 0xf2 }, 
    { 0xa4, 0x4a, 0x82, 0x66, 0xee, 0x1c, 0x8e, 0xb0,
      0xc8, 0xb5, 0xd4, 0xcf, 0x5a, 0xe9, 0xf1, 0x9a }, 
};

int gcm_self_test( int verbose )
{
    gcm_context ctx;
    unsigned char buf[64];
    unsigned char tag_buf[16];
    int i, j, ret;

    for( j = 0; j < 3; j++ )
    {
        int key_len = 128 + 64 * j;

        for( i = 0; i < MAX_TESTS; i++ )
        {
            printf( "  AES-GCM-%3d #%d (%s): ", key_len, i, "enc" );
            gcm_init( &ctx, key[key_index[i]], key_len );

            ret = gcm_crypt_and_tag( &ctx, GCM_ENCRYPT,
                                     pt_len[i],
                                     iv[iv_index[i]], iv_len[i],
                                     additional[add_index[i]], add_len[i],
                                     pt[pt_index[i]], buf, 16, tag_buf );

            if( ret != 0 ||
                memcmp( buf, ct[j * 6 + i], pt_len[i] ) != 0 ||
                memcmp( tag_buf, tag[j * 6 + i], 16 ) != 0 )
            {
                if( verbose != 0 )
                    printf( "failed\n" );

                return( 1 );
            }

            if( verbose != 0 )
                printf( "passed\n" );

            printf( "  AES-GCM-%3d #%d (%s): ", key_len, i, "dec" );
            gcm_init( &ctx, key[key_index[i]], key_len );

            ret = gcm_crypt_and_tag( &ctx, GCM_DECRYPT,
                                     pt_len[i],
                                     iv[iv_index[i]], iv_len[i],
                                     additional[add_index[i]], add_len[i],
                                     ct[j * 6 + i], buf, 16, tag_buf );

            if( ret != 0 ||
                memcmp( buf, pt[pt_index[i]], pt_len[i] ) != 0 ||
                memcmp( tag_buf, tag[j * 6 + i], 16 ) != 0 )
            {
                if( verbose != 0 )
                    printf( "failed\n" );

                return( 1 );
            }

            if( verbose != 0 )
                printf( "passed\n" );
        }
    }
    
    printf( "\n" );

    return( 0 );
}

#endif

#endif
