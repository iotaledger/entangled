/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_PTRIT_H_
#define __COMMON_CURL_P_PTRIT_H_

#include "common/crypto/curl-p/const.h"
#include "common/trinary/ptrit.h"
#include "utils/memset_safe.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CURL_RATE 243
#define CURL_STATE_SIZE (3 * CURL_RATE)

typedef struct {
#if defined(PCURL_STATE_SHORT)
  ptrit_t state[3 * (CURL_STATE_SIZE + 1) / 2];
#elif defined(PCURL_STATE_DOUBLE)
  ptrit_t state[2 * CURL_STATE_SIZE];
#else
#error Invalid PCURL_STATE.
#endif
  size_t round_count;
} pcurl_t;

void pcurl_init(pcurl_t *ctx, size_t round_count);
void pcurl_absorb(pcurl_t *ctx, ptrit_t const *ptrits, size_t length);
void pcurl_squeeze(pcurl_t *ctx, ptrit_t *ptrits, size_t length);
void pcurl_transform(pcurl_t *ctx);
void pcurl_reset(pcurl_t *ctx);

void pcurl_get_hash(pcurl_t *ctx, ptrit_t *hash);
void pcurl_hash_data(pcurl_t *ctx, ptrit_t const *data, size_t size, ptrit_t *hash);

typedef pcurl_t PCurl;
void ptrit_curl_init(PCurl *const ctx, CurlType type);
void ptrit_curl_absorb(PCurl *const ctx, ptrit_t const *const trits, size_t length);
void ptrit_curl_squeeze(PCurl *const ctx, ptrit_t *const trits, size_t length);
void ptrit_transform(PCurl *const ctx);
void ptrit_curl_reset(PCurl *const ctx);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CURL_P_PTRIT_H_
