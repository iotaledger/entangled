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

#include "iss.h.inc"

#undef HASH_PREFIX
#undef HASH_STATE

#endif /* __ISS_KERL_H_ */
