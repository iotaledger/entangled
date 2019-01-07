/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_TRIT_H_
#define __COMMON_CURL_P_TRIT_H_

#include <string.h>

#include "common/curl-p/const.h"
#include "common/curl-p/indices.h"
#include "common/trinary/trits.h"
#include "utils/forced_inline.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  trit_t state[STATE_LENGTH];
  CurlType type;
} Curl;

#define __TRUTH_TABLE 1, 0, -1, 2, 1, -1, 0, 2, -1, 1, 0

static const size_t CURL_INDEX[STATE_LENGTH + 1] = {__INDEX_TABLE};

static const trit_t TRUTH_TABLE[11] = {__TRUTH_TABLE};

static FORCED_INLINE void sbox(Curl* const c, Curl* const s) {
  size_t i = 0;
  for (; i < STATE_LENGTH; ++i) {
    c->state[i] = TRUTH_TABLE[s->state[CURL_INDEX[i]] +
                              ((unsigned)s->state[CURL_INDEX[i + 1]] << 2) + 5];
  }
}

void init_curl(Curl* ctx);
void curl_absorb(Curl* const ctx, trit_t const* const trits, size_t length);
void curl_squeeze(Curl* const ctx, trit_t* const trits, size_t length);
void curl_reset(Curl* const ctx);

#endif
#ifdef __cplusplus
}

#endif  // __COMMON_CURL_P_TRIT_H_
