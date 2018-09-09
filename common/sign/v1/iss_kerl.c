/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "iss_kerl.h"
#include "common/kerl/kerl.h"

#define HASH_PREFIX kerl
#define HASH_STATE Kerl

#include "iss.c.inc"

#undef HASH_PREFIX
#undef HASH_STATE
