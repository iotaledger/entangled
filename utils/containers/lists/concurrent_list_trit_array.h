/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_TRIT_ARRAY_H__
#define __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_TRIT_ARRAY_H__

#include "common/trinary/trit_array.h"
#include "utils/containers/lists/concurrent_list.h.inc"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_CL(trit_array_p);

typedef concurrent_list_trit_array_p trit_array_list_t;

bool trit_array_cmp(trit_array_p const *const lhs,
                    trit_array_p const *const rhs);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_TRIT_ARRAY_H__
