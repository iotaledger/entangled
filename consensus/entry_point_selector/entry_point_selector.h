/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_ENTRY_POINT_SELECTOR_ENTRY_POINT_SELECTOR_H__
#define __CONSENSUS_ENTRY_POINT_SELECTOR_ENTRY_POINT_SELECTOR_H__

#include <stdbool.h>

#include "common/errors.h"

// Forward declarations
typedef struct milestone_s milestone_t;
typedef struct tangle_s tangle_t;
typedef struct _trit_array *trit_array_p;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct entry_point_selector_s {
  milestone_t *milestone;
  tangle_t *tangle;
  bool testnet;
} entry_point_selector_t;

extern retcode_t iota_consensus_entry_point_selector_init(
    entry_point_selector_t *const eps, milestone_t *const milestone,
    tangle_t *const tangle, bool const testnet);

extern retcode_t iota_consensus_get_entry_point(
    entry_point_selector_t *const eps, size_t const depth,
    trit_array_p const ep);

extern retcode_t iota_consensus_entry_point_selector_destroy(
    entry_point_selector_t *const eps);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_ENTRY_POINT_SELECTOR_ENTRY_POINT_SELECTOR_H__
