/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_ATTACH_TO_TANGLE_H
#define CCLIENT_RESPONSE_ATTACH_TO_TANGLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

// response is a different set of tryte values which you can input into
// broadcastTransactions and storeTransactions.
typedef UT_array attach_to_tangle_res_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_ATTACH_TO_TANGLE_H