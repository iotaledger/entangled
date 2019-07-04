/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_ENTRY_POINT_SELECTOR_ENTRY_POINT_SELECTOR_H__
#define __CONSENSUS_ENTRY_POINT_SELECTOR_ENTRY_POINT_SELECTOR_H__

#include <stdbool.h>

#include "ciri/consensus/ledger_validator/ledger_validator.h"
#include "common/errors.h"

// Forward declarations
typedef struct milestone_tracker_s milestone_tracker_t;
typedef struct tangle_s tangle_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct entry_point_selector_s {
  milestone_tracker_t *mt;
} entry_point_selector_t;

/**
 * Initializes an entry point selector
 *
 * @param eps The entry point selector
 * @param mt A milestone tracker
 *
 * @return a status code
 */
retcode_t iota_consensus_entry_point_selector_init(entry_point_selector_t *const eps, milestone_tracker_t *const mt);

/**
 * Gets an entry point for the random walk.
 * Given a depth N, returns a N-deep milestone, i.e. (latest_solid - depth)th
 * milestone.
 *
 * @param eps The entry point selector
 * @param tangle A tangle
 * @param depth The depth
 * @param ep The entry point
 *
 * @return a status code
 */
retcode_t iota_consensus_entry_point_selector_get_entry_point(entry_point_selector_t *const eps, tangle_t *const tangle,
                                                              uint32_t const depth, flex_trit_t *const ep);

/**
 * Destroys an entry point selector
 *
 * @param eps The entry point selector
 *
 * @return a status code
 */
retcode_t iota_consensus_entry_point_selector_destroy(entry_point_selector_t *const eps);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_ENTRY_POINT_SELECTOR_ENTRY_POINT_SELECTOR_H__
