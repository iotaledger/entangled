/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CRYPTO_SPONGE_SPONGE_H__
#define __COMMON_CRYPTO_SPONGE_SPONGE_H__

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum sponge_type_e {
  SPONGE_CURLP27,
  SPONGE_CURLP81,
  SPONGE_KERL,
  SPONGE_TROIKA,
  SPONGE_UNKNOWN,
} sponge_type_t;

typedef void (*sponge_absorb_t)(void* const state, trit_t const* const input, size_t const length);
typedef void (*sponge_squeeze_t)(void* const state, trit_t* const output, size_t const length);
typedef void (*sponge_reset_t)(void* const state);

typedef struct sponge_s {
  sponge_type_t type;
  sponge_absorb_t absorb;
  sponge_squeeze_t squeeze;
  sponge_reset_t reset;
  void* state;
} sponge_t;

retcode_t sponge_init(sponge_t* const sponge, sponge_type_t const type);
retcode_t sponge_absorb(sponge_t* const sponge, trit_t const* const input, size_t const length);
retcode_t sponge_squeeze(sponge_t* const sponge, trit_t* const output, size_t const length);
retcode_t sponge_reset(sponge_t* const sponge);
retcode_t sponge_destroy(sponge_t* const sponge);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CRYPTO_SPONGE_SPONGE_H__
