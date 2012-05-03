#ifndef _DES_H
#define _DES_H

/*
MSN Protocol plugin for
Miranda IM: the free IM client for Microsoft* Windows*

Authors 
			Copyright (c) 2006-2012 Boris Krasnovskiy.
			Copyright (c) 2003-2005 George Hazan.
			Copyright (c) 2002-2003 Richard Hughes (original version).

Copyright 2000-2012 Miranda IM project,
all portions of this codebase are copyrighted to the people
listed in contributors.txt.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

===============================================================================

File name      : $HeadURL: 
Revision       : $Revision: 
Last change on : $Date: 
Last change by : $Author:
$Id$		   : $Id$:

===============================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          DES context structure
 */
typedef struct
{
    unsigned long esk[32];     /*!< DES encryption subkeys */
    unsigned long dsk[32];     /*!< DES decryption subkeys */
}
des_context;

/**
 * \brief          Triple-DES context structure
 */
typedef struct
{
    unsigned long esk[96];     /*!< Triple-DES encryption subkeys */
    unsigned long dsk[96];     /*!< Triple-DES decryption subkeys */
}
des3_context;

/**
 * \brief          DES key schedule (56-bit)
 *
 * \param ctx      DES context to be initialized
 * \param key      8-byte secret key
 */
void des_set_key( des_context *ctx, unsigned char key[8] );

/**
 * \brief          DES block encryption (ECB mode)
 *
 * \param ctx      DES context
 * \param input    plaintext  block
 * \param output   ciphertext block
 */
void des_encrypt( des_context *ctx,
                  unsigned char input[8],
                  unsigned char output[8] );

/**
 * \brief          DES block decryption (ECB mode)
 *
 * \param ctx      DES context
 * \param input    ciphertext block
 * \param output   plaintext  block
 */
void des_decrypt( des_context *ctx,
                  unsigned char input[8],
                  unsigned char output[8] );

/**
 * \brief          DES-CBC buffer encryption
 *
 * \param ctx      DES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the plaintext
 * \param output   buffer holding the ciphertext
 * \param len      length of the data to be encrypted
 */
void des_cbc_encrypt( des_context *ctx,
                      unsigned char iv[8],
                      unsigned char *input,
                      unsigned char *output,
                      int len );

/**
 * \brief          DES-CBC buffer decryption
 *
 * \param ctx      DES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the ciphertext
 * \param output   buffer holding the plaintext
 * \param len      length of the data to be decrypted
 */
void des_cbc_decrypt( des_context *ctx,
                      unsigned char iv[8],
                      unsigned char *input,
                      unsigned char *output,
                      int len );

/**
 * \brief          Triple-DES key schedule (112-bit)
 *
 * \param ctx      3DES context to be initialized
 * \param key      16-byte secret key
 */
void des3_set_2keys( des3_context *ctx, unsigned char key[16] );

/**
 * \brief          Triple-DES key schedule (168-bit)
 *
 * \param ctx      3DES context to be initialized
 * \param key      24-byte secret key
 */
void des3_set_3keys( des3_context *ctx, unsigned char key[24] );

/**
 * \brief          Triple-DES block encryption (ECB mode)
 *
 * \param ctx      3DES context
 * \param input    plaintext  block
 * \param output   ciphertext block
 */
void des3_encrypt( des3_context *ctx,
                   unsigned char input[8],
                   unsigned char output[8] );

/**
 * \brief          Triple-DES block decryption (ECB mode)
 *
 * \param ctx      3DES context
 * \param input    ciphertext block
 * \param output   plaintext  block
 */
void des3_decrypt( des3_context *ctx,
                   unsigned char input[8],
                   unsigned char output[8] );

/**
 * \brief          3DES-CBC buffer encryption
 *
 * \param ctx      3DES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the plaintext
 * \param output   buffer holding the ciphertext
 * \param len      length of the data to be encrypted
 */
void des3_cbc_encrypt( des3_context *ctx,
                       unsigned char iv[8],
                       unsigned char *input,
                       unsigned char *output,
                       int len );

/**
 * \brief          3DES-CBC buffer decryption
 *
 * \param ctx      3DES context
 * \param iv       initialization vector (modified after use)
 * \param input    buffer holding the ciphertext
 * \param output   buffer holding the plaintext
 * \param len      length of the data to be decrypted
 */
void des3_cbc_decrypt( des3_context *ctx,
                       unsigned char iv[8],
                       unsigned char *input,
                       unsigned char *output,
                       int len );

/*
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int des_self_test( int verbose );

#ifdef __cplusplus
}
#endif

#endif /* des.h */
