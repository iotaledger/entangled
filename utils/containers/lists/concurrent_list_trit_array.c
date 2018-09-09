/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "utils/containers/lists/concurrent_list_trit_array.h"
#include "common/trinary/trit_array.h"
#include "utils/containers/lists/concurrent_list.c.inc"

DEFINE_CL(trit_array_p);

bool trit_array_cmp(trit_array_p const *const lhs,
                    trit_array_p const *const rhs) {
  if (lhs == NULL || rhs == NULL) {
    return false;
  }
  return (*lhs)->num_bytes == (*rhs)->num_bytes &&
         memcmp((*lhs)->trits, (*rhs)->trits, (*lhs)->num_bytes);
}
