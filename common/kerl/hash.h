/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __KERL_HASH_H
#define __KERL_HASH_H

#include "common/kerl/kerl.h"

void kerl_hash(const trit_t* const trits, size_t len, trit_t* out, Kerl* kerl);

#endif  //__KERL_HASH_H
#ifdef __cplusplus
}
#endif
