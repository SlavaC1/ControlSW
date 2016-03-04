/*
 -------------------------------------------------------------------------
 Copyright (c) 2001, Dr Brian Gladman <brg@gladman.me.uk>, Worcester, UK.
 All rights reserved.

 LICENSE TERMS

 The free distribution and use of this software in both source and binary 
 form is allowed (with or without changes) provided that:

   1. distributions of this source code include the above copyright 
      notice, this list of conditions and the following disclaimer;

   2. distributions in binary form include the above copyright
      notice, this list of conditions and the following disclaimer
      in the documentation and/or other associated materials;

   3. the copyright holder's name is not used to endorse products 
      built using this software without specific written permission. 

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness 
 and fitness for purpose.
 -------------------------------------------------------------------------
Issue Date: 9/10/2002
*/

#ifndef _SHA2_H
#define _SHA2_H

#include <limits.h>

/*  Defines for suffixes to 32 and 64 bit unsigned numeric values	*/

#define sfx_lo(x,y) x##y
#define sfx_hi(x,y) sfx_lo(x,y)
#define n_u32(p)    sfx_hi(0x##p,s_u32)
#define n_u64(p)    sfx_hi(0x##p,s_u64)

/* define an unsigned 32-bit type */

#if UINT_MAX == 0xffffffff
  typedef   unsigned int     sha_32t;
  #define s_u32    u
#elif ULONG_MAX == 0xffffffff
  typedef   unsigned long    sha_32t;
  #define s_u32   ul
#else
#error Please define sha_32t as an unsigned 32 bit type in sha2.h
#endif

/* define an unsigned 64-bit type */

#if defined( _MSC_VER )
  typedef unsigned __int64   sha_64t;
  #define s_u64 ui64
#elif ULONG_MAX == 0xffffffffffffffff
  typedef unsigned long      sha_64t;
  #define s_u64   ul
#elif ULONG_MAX == 0xffffffff
  typedef unsigned long long sha_64t;	/* a somewhat dangerous guess */
  #define s_u64  ull
#else
#error Please define sha_64t as an unsigned 64 bit type in sha2.h
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

#define SHA256_DIGEST_LENGTH    32
#define SHA384_DIGEST_LENGTH    48
#define SHA512_DIGEST_LENGTH    64

#define SHA2_DIGEST_LENGTH      SHA256_DIGEST_LENGTH
#define SHA2_MAX_DIGEST_LENGTH  SHA512_DIGEST_LENGTH

#define SHA2_GOOD   0
#define SHA2_BAD    1

/* type to hold the SHA256 context  */

typedef struct
{   sha_32t count[2];
    sha_32t hash[8];
    sha_32t wdat[16];
} sha256_ctx;

typedef struct
{   sha_64t count[2];
    sha_64t hash[8];
    sha_64t wdat[16];
} sha512_ctx;

typedef sha512_ctx  sha384_ctx;

typedef struct
{   union
    {   sha256_ctx  ctx256[1];
        sha512_ctx  ctx512[1];
    } uu[1];
    sha_32t    sha2_len;
} sha2_ctx;

void sha256_begin(sha256_ctx ctx[1]);
void sha256_hash(const unsigned char data[], const unsigned long len, sha256_ctx ctx[1]);
void sha256_end(unsigned char hval[], sha256_ctx ctx[1]);

void sha384_begin(sha384_ctx ctx[1]);
#define sha384_hash sha512_hash
void sha384_end(unsigned char hval[], sha384_ctx ctx[1]);

void sha512_begin(sha512_ctx ctx[1]);
void sha512_hash(const unsigned char data[], const unsigned long len, sha512_ctx ctx[1]);
void sha512_end(unsigned char hval[], sha512_ctx ctx[1]);

int sha2_begin(const unsigned long len, sha2_ctx ctx[1]);
void sha2_hash(const unsigned char data[], const unsigned long len, sha2_ctx ctx[1]);
void sha2_end(unsigned char hval[], sha2_ctx ctx[1]);

#if defined(__cplusplus)
}
#endif

#endif
