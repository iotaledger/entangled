/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/curl-p/trit.h"

#define HASH_PREFIX curl
#define HASH_STATE Curl

#include "iss.c.inc"

#undef HASH_PREFIX
#undef HASH_STATE
