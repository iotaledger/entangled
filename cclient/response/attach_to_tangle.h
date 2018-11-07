/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_ATTACH_TO_TANGLE_H
#define CCLIENT_RESPONSE_ATTACH_TO_TANGLE_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /**
   * The returned value is a different set of tryte values which you can input
   * into broadcastTransactions and storeTransactions
   */
  hash8019_queue_t trytes;

} attach_to_tangle_res_t;

attach_to_tangle_res_t* attach_to_tangle_res_new();
flex_trit_t* attach_to_tangle_res_trytes_at(attach_to_tangle_res_t* res,
                                            int index);
size_t attach_to_tangle_res_trytes_cnt(attach_to_tangle_res_t* res);
void attach_to_tangle_res_free(attach_to_tangle_res_t** hashes);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_ATTACH_TO_TANGLE_H