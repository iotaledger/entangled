/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_TRIT_ARRAY_H__
#define __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_TRIT_ARRAY_H__

#include "utils/containers/lists/concurrent_list.h.inc"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct _trit_array *trit_array_p;

DECLARE_CL(trit_array_p);

/**
 * trit_array_p comparator for list operations
 *
 * @param lhs Left hand side trit_array_p
 * @param rhs Right hand side trit_array_p
 *
 * @return true if equal, false otherwise
 */
bool trit_array_cmp(trit_array_p const *const lhs,
                    trit_array_p const *const rhs);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_CONTAINERS_LISTS_CONCURRENT_LIST_TRIT_ARRAY_H__
