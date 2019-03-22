/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __KERL_HASH_H
#define __KERL_HASH_H

#include "common/crypto/kerl/kerl.h"

#ifdef __cplusplus
extern "C" {
#endif

void kerl_hash(const trit_t* const trits, size_t len, trit_t* out, Kerl* kerl);

#ifdef __cplusplus
}
#endif

#endif  //__KERL_HASH_H
