/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __ISS_KERL_H_
#define __ISS_KERL_H_

#include "common/kerl/kerl.h"

#define HASH_PREFIX kerl
#define HASH_STATE Kerl

#ifdef __cplusplus
extern "C" {
#endif

#include "iss.h.inc"

#ifdef __cplusplus
}
#endif

#undef HASH_PREFIX
#undef HASH_STATE

#endif /* __ISS_KERL_H_ */
