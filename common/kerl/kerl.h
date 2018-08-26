/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_KERL_KERL_H_
#define __COMMON_KERL_KERL_H_

#include <keccak/KeccakHash.h>

#include "common/curl-p/const.h"
#include "common/stdint.h"
#include "common/trinary/trits.h"

typedef struct {
  Keccak_HashInstance keccak;
} Kerl;

void init_kerl(Kerl* const ctx);

void kerl_absorb(Kerl* const ctx, trit_t const* trits, size_t const length);
void kerl_squeeze(Kerl* const ctx, trit_t* trits, size_t const length);
void kerl_reset(Kerl* const ctx);

#endif
#ifdef __cplusplus
}
#endif
