/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <math.h>

#include "common/crypto/iss/normalize.h"
#include "common/crypto/kerl/kerl.h"
#include "common/defs.h"
#include "common/model/transaction.h"
#include "common/trinary/trit_long.h"
#include "utils/bundle_miner.h"
#include "utils/handles/thread.h"
#include "utils/macros.h"
#include "utils/system.h"

#define OBSOLETE_TAG_OFFSET (NUM_TRITS_ADDRESS + NUM_TRITS_VALUE)

/**
 * Private functions
 */

static void *bundle_miner_mine_routine(void *const param) {
  Kerl kerl;
  double probability = 0.0;
  trit_t candidate[HASH_LENGTH_TRIT];
  byte_t candidate_normalized[NORMALIZED_BUNDLE_LENGTH];
  byte_t candidate_normalized_max[NORMALIZED_BUNDLE_LENGTH];
  bundle_miner_ctx_t *ctx = (bundle_miner_ctx_t *)param;
  uint64_t num_trials_mining_threshold = pow(3, ctx->mining_threshold);

  kerl_init(&kerl);

  for (size_t i = 0; i < ctx->count; i++) {
    if (ctx->optimal_index_found_by_some_thread && *ctx->optimal_index_found_by_some_thread) {
      break;
    }
    long_to_trits(ctx->index, ctx->essence + OBSOLETE_TAG_OFFSET);

    kerl_reset(&kerl);
    kerl_absorb(&kerl, ctx->essence, ctx->essence_length);
    kerl_squeeze(&kerl, candidate, HASH_LENGTH_TRIT);

    normalize_hash(candidate, candidate_normalized);

    if (normalized_hash_is_secure(candidate_normalized, ctx->security * NORMALIZED_FRAGMENT_LENGTH)) {
      bundle_miner_normalized_bundle_max(ctx->bundle_normalized_max, candidate_normalized, candidate_normalized_max,
                                         ctx->security * NORMALIZED_FRAGMENT_LENGTH);
      probability = bundle_miner_probability_of_losing(candidate_normalized_max, ctx->security);
      if (probability < ctx->probability) {
        ctx->probability = probability;
        ctx->optimal_index = ctx->index;
        if (num_trials_mining_threshold > 1 && (uint64_t)(1.0L / probability) >= num_trials_mining_threshold) {
          if (ctx->optimal_index_found_by_some_thread) {
            *ctx->optimal_index_found_by_some_thread = true;
          }
          break;
        }
      }
    }

    ctx->index += 1;
  }

  return NULL;
}

/**
 * Public functions
 */

double bundle_miner_probability_of_losing(byte_t const *const normalized_hash, uint8_t const security) {
  double p = 0.0;
  double pi = 0.0;

  for (size_t i = 0; i < security * NORMALIZED_FRAGMENT_LENGTH; i++) {
    pi = 1.0 - ((double)(TRYTE_VALUE_MAX - normalized_hash[i]) / (double)(TRYTE_VALUE_MAX - TRYTE_VALUE_MIN + 1));
    if (pi > 0.0) {
      if (p == 0.0) {
        p = 1.0;
      }
      p *= pi;
    }
  }

  return p;
}

void bundle_miner_normalized_bundle_max(byte_t const *const lhs, byte_t const *const rhs, byte_t *const max,
                                        size_t const length) {
  for (size_t i = 0; i < length; i++) {
    max[i] = MAX(lhs[i], rhs[i]);
  }
}

retcode_t bundle_miner_mine(byte_t const *const bundle_normalized_max, uint8_t const security,
                            trit_t const *const essence, size_t const essence_length, uint32_t const count,
                            uint64_t mining_threshold, uint64_t *const index, bundle_miner_ctx_t *const ctxs,
                            size_t num_ctxs, bool *const optimal_index_found) {
  retcode_t ret = RC_OK;
  uint32_t rounded_count = count + (num_ctxs - count % num_ctxs);
  thread_handle_t *threads = (thread_handle_t *)malloc(sizeof(thread_handle_t) * num_ctxs);
  uint64_t start_index = 0;
  double probability = 1.0;

  if (bundle_normalized_max == NULL || essence == NULL || index == NULL) {
    return RC_NULL_PARAM;
  }

  if (security > 3 || essence_length % HASH_LENGTH_TRIT != 0) {
    return RC_UTILS_BUNDLE_MINER_BAD_PARAM;
  }

  if (ctxs == NULL || threads == NULL) {
    return RC_OOM;
  }

  start_index = trits_to_long(essence + OBSOLETE_TAG_OFFSET, NUM_TRITS_OBSOLETE_TAG);
  *index = 0;

  for (size_t i = 0; i < num_ctxs; i++) {
    ctxs[i].bundle_normalized_max = bundle_normalized_max;
    ctxs[i].security = security;
    if ((ctxs[i].essence = (trit_t *)malloc(sizeof(trit_t) * essence_length)) == NULL) {
      ret = RC_OOM;
      goto done;
    }
    memcpy(ctxs[i].essence, essence, essence_length);
    ctxs[i].essence_length = essence_length;
    ctxs[i].start_index = start_index + (i * (rounded_count / num_ctxs));
    ctxs[i].index = ctxs[i].start_index;
    ctxs[i].count = rounded_count / num_ctxs;
    ctxs[i].optimal_index = 0;
    ctxs[i].probability = 1.0;
    ctxs[i].mining_threshold = mining_threshold;
    ctxs[i].optimal_index_found_by_some_thread = optimal_index_found;
    thread_handle_create(&threads[i], (thread_routine_t)bundle_miner_mine_routine, &ctxs[i]);
    ctxs[i].was_thread_created = true;
  }

  for (size_t i = 0; i < num_ctxs; i++) {
    thread_handle_join(threads[i], NULL);
    if (ctxs[i].probability < probability) {
      probability = ctxs[i].probability;
      *index = ctxs[i].optimal_index;
    }
    free(ctxs[i].essence);
  }

done:

  free(threads);

  return ret;
}

retcode_t bundle_miner_allocate_ctxs(uint8_t const nprocs, bundle_miner_ctx_t **const ctxs, size_t *num_ctxs) {
  *num_ctxs = nprocs == 0 ? system_cpu_available() : nprocs;
  *ctxs = (bundle_miner_ctx_t *)malloc(sizeof(bundle_miner_ctx_t) * (*num_ctxs));
  if (*ctxs == NULL) {
    return RC_OOM;
  }

  for (size_t i = 0; i < *num_ctxs; i++) {
    (*ctxs)[i].was_thread_created = false;
  }

  return RC_OK;
}

void bundle_miner_deallocate_ctxs(bundle_miner_ctx_t **const ctxs) { free(*ctxs); }

float bundle_miner_get_progress_ratio(bundle_miner_ctx_t const *const ctxs, size_t num_ctxs) {
  double progress = 0;
  double total_count = 0;
  for (size_t i = 0; i < num_ctxs; i++) {
    // If not all threads have already made some progress
    // let's consider that as 0 progress to avoid undefined behavior
    if (!ctxs[i].was_thread_created) {
      return 0;
    }
  }

  for (size_t i = 0; i < num_ctxs; i++) {
    // If we hit the threshold, we're finished
    if (ctxs[i].optimal_index_found_by_some_thread) {
      return 1.0;
    }
  }

  for (size_t i = 0; i < num_ctxs; i++) {
    progress += ctxs[i].index - ctxs[i].start_index;
    total_count += ctxs[i].count;
  }

  return progress / total_count;
}
