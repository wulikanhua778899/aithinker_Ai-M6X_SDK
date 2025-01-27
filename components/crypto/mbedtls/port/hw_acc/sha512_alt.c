/*
 *  FIPS-180-2 compliant SHA-384/512 implementation
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*
 *  The SHA-512 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#include "common.h"

#if defined(MBEDTLS_SHA512_C)

#include "mbedtls/sha512.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#if defined(_MSC_VER) || defined(__WATCOMC__)
  #define UL64(x) x##ui64
#else
  #define UL64(x) x##ULL
#endif

#include <string.h>

#include "mbedtls/platform.h"
#include "sec_mutex.h"

#define SHA512_VALIDATE_RET(cond)                           \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_SHA512_BAD_INPUT_DATA )
#define SHA512_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE( cond )

void mbedtls_sha512_init( mbedtls_sha512_context *ctx )
{
    SHA512_VALIDATE( ctx != NULL );

    struct bflb_device_s *sha;

    sha = bflb_device_get_by_name("sha");

    bflb_l1c_dcache_clean_invalidate_range((void*)ctx, sizeof(*ctx));
    ctx = bflb_get_no_cache_addr(ctx);

    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_sha512_context ) );
    ctx->sha = sha;

    bflb_group0_request_sha_access(sha);
    bflb_sha_link_init(sha);
}

void mbedtls_sha512_free( mbedtls_sha512_context *ctx )
{
    if( ctx == NULL )
        return;

    ctx = bflb_get_no_cache_addr(ctx);
    
    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_sha512_context ) );
}

void mbedtls_sha512_clone( mbedtls_sha512_context *dst,
                           const mbedtls_sha512_context *src )
{
    SHA512_VALIDATE( dst != NULL );
    SHA512_VALIDATE( src != NULL );

    *dst = *src;
    bflb_l1c_dcache_clean_invalidate_range((void*)dst, sizeof(*dst));
}

/*
 * SHA-512 context setup
 */
int mbedtls_sha512_starts_ret( mbedtls_sha512_context *ctx, int is384 )
{
    SHA512_VALIDATE_RET( ctx != NULL );
#if !defined(MBEDTLS_SHA512_NO_SHA384)
    SHA512_VALIDATE_RET( is384 == 0 || is384 == 1 );
#else
    SHA512_VALIDATE_RET( is384 == 0 );
#endif

    ctx = bflb_get_no_cache_addr(ctx);

    bflb_sha512_link_start(ctx->sha, &ctx->link_ctx, is384);
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha512_starts( mbedtls_sha512_context *ctx,
                            int is384 )
{
    mbedtls_sha512_starts_ret( ctx, is384 );
}
#endif

/*
 * SHA-512 process buffer
 */
int mbedtls_sha512_update_ret( mbedtls_sha512_context *ctx,
                               const unsigned char *input,
                               size_t ilen )
{
    //int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    SHA512_VALIDATE_RET( ctx != NULL );
    SHA512_VALIDATE_RET( ilen == 0 || input != NULL );

    if( ilen == 0 )
        return( 0 );

    ctx = bflb_get_no_cache_addr(ctx);

    bflb_l1c_dcache_clean_range((void *)input, ilen);
    bflb_sec_sha_mutex_take();
    bflb_sha512_link_update(ctx->sha, &ctx->link_ctx, input, ilen);
    bflb_sec_sha_mutex_give();
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha512_update( mbedtls_sha512_context *ctx,
                            const unsigned char *input,
                            size_t ilen )
{
    mbedtls_sha512_update_ret( ctx, input, ilen );
}
#endif

/*
 * SHA-512 final digest
 */
int mbedtls_sha512_finish_ret( mbedtls_sha512_context *ctx,
                               unsigned char output[64] )
{
    //int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    SHA512_VALIDATE_RET( ctx != NULL );
    SHA512_VALIDATE_RET( (unsigned char *)output != NULL );

    ctx = bflb_get_no_cache_addr(ctx);

    bflb_sec_sha_mutex_take();
    bflb_sha512_link_finish(ctx->sha, &ctx->link_ctx, output);
    bflb_sec_sha_mutex_give();
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha512_finish( mbedtls_sha512_context *ctx,
                            unsigned char output[64] )
{
    mbedtls_sha512_finish_ret( ctx, output );
}
#endif

#endif /* MBEDTLS_SHA512_C */
