/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_STORAGE_H__
#define __COMMON_STORAGE_STORAGE_H__

#include <stdbool.h>

#include "ciri/consensus/snapshot/state_delta.h"
#include "common/errors.h"
#include "common/model/bundle.h"
#include "common/storage/connection.h"
#include "common/storage/defs.h"
#include "common/storage/pack.h"
#include "common/trinary/flex_trit.h"
#include "utils/containers/hash/hash243_queue.h"
#include "utils/containers/hash/hash243_set.h"
#include "utils/containers/hash/hash81_queue.h"

// Forward declaration
typedef struct iota_milestone_s iota_milestone_t;
typedef struct iota_transaction_s iota_transaction_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the storage backend
 * Should only be called once per process
 *
 * @return a status code
 */
extern retcode_t storage_init();

/**
 * Destroys the storage backend
 * Should only be called once per process
 *
 * @return a status code
 */
extern retcode_t storage_destroy();

/*
 * Transaction operations
 */

typedef enum storage_transaction_field_e {
  TRANSACTION_FIELD_NONE,
  TRANSACTION_FIELD_HASH,
  TRANSACTION_FIELD_ADDRESS
} storage_transaction_field_t;

typedef enum storage_load_model_e {
  MODEL_HASH,
  MODEL_MILESTONE,
  MODEL_TRANSACTION,
  MODEL_TRANSACTION_ESSENCE_METADATA,
  MODEL_TRANSACTION_ESSENCE_ATTACHMENT_METADATA,
  MODEL_TRANSACTION_ESSENCE_CONSENSUS,
  MODEL_TRANSACTION_METADATA,
} storage_load_model_t;

typedef enum storage_value_type_e {
  BOOLEAN,
  INT64,
} storage_value_type_t;

extern retcode_t storage_transaction_count(storage_connection_t const* const connection, size_t* const count);

extern retcode_t storage_transaction_store(storage_connection_t const* const connection,
                                           iota_transaction_t const* const transaction);

extern retcode_t storage_transaction_load(storage_connection_t const* const connection,
                                          storage_transaction_field_t const field, flex_trit_t const* const key,
                                          iota_stor_pack_t* const pack);

extern retcode_t storage_transaction_load_essence_metadata(storage_connection_t const* const connection,
                                                           flex_trit_t const* const hash, iota_stor_pack_t* const pack);

extern retcode_t storage_transaction_load_essence_attachment_metadata(storage_connection_t const* const connection,
                                                                      flex_trit_t const* const hash,
                                                                      iota_stor_pack_t* const pack);

extern retcode_t storage_transaction_load_essence_consensus(storage_connection_t const* const connection,
                                                            flex_trit_t const* const hash,
                                                            iota_stor_pack_t* const pack);

extern retcode_t storage_transaction_load_metadata(storage_connection_t const* const connection,
                                                   flex_trit_t const* const hash, iota_stor_pack_t* const pack);

extern retcode_t storage_transaction_exist(storage_connection_t const* const connection,
                                           storage_transaction_field_t const field, flex_trit_t const* const key,
                                           bool* const exist);

extern retcode_t storage_transaction_update_snapshot_index(storage_connection_t const* const connection,
                                                           flex_trit_t const* const hash,
                                                           uint64_t const snapshot_index);

extern retcode_t storage_transaction_update_solidity(storage_connection_t const* const connection,
                                                     flex_trit_t const* const hash, bool const is_solid);

extern retcode_t storage_transaction_update_validity(storage_connection_t const* const connection,
                                                     flex_trit_t const* const hash, bundle_status_t const validity);

extern retcode_t storage_transaction_load_hashes(storage_connection_t const* const connection,
                                                 storage_transaction_field_t const field, flex_trit_t const* const key,
                                                 iota_stor_pack_t* const pack);

extern retcode_t storage_transaction_load_hashes_of_approvers(storage_connection_t const* const connection,
                                                              flex_trit_t const* const approvee_hash,
                                                              iota_stor_pack_t* const pack, int64_t before_timestamp);

extern retcode_t storage_transaction_load_hashes_of_milestone_candidates(storage_connection_t const* const connection,
                                                                         iota_stor_pack_t* const pack,
                                                                         flex_trit_t const* const coordinator);

extern retcode_t storage_transaction_approvers_count(storage_connection_t const* const connection,
                                                     flex_trit_t const* const hash, size_t* const count);

extern retcode_t storage_transaction_find(storage_connection_t const* const connection, hash243_queue_t const bundles,
                                          hash243_queue_t const addresses, hash81_queue_t const tags,
                                          hash243_queue_t const approvees, iota_stor_pack_t* const pack);

extern retcode_t storage_transaction_metadata_clear(storage_connection_t const* const connection);

extern retcode_t storage_transactions_update_snapshot_index(storage_connection_t const* const connection,
                                                            hash243_set_t const hashes, uint64_t const snapshot_index);

extern retcode_t storage_transactions_update_solidity(storage_connection_t const* const connection,
                                                      hash243_set_t const hashes, bool const is_solid);

extern retcode_t storage_transactions_delete(storage_connection_t const* const connection, hash243_set_t const hashes);

/*
 * Bundle operations
 */

extern retcode_t storage_bundle_update_validity(storage_connection_t const* const connection,
                                                bundle_transactions_t const* const bundle,
                                                bundle_status_t const status);

/*
 * Milestone operations
 */

extern retcode_t storage_milestone_clear(storage_connection_t const* const connection);

extern retcode_t storage_milestone_store(storage_connection_t const* const connection,
                                         iota_milestone_t const* const milestone);

extern retcode_t storage_milestone_load(storage_connection_t const* const connection, flex_trit_t const* const hash,
                                        iota_stor_pack_t* const pack);

extern retcode_t storage_milestone_load_last(storage_connection_t const* const connection,
                                             iota_stor_pack_t* const pack);

extern retcode_t storage_milestone_load_first(storage_connection_t const* const connection,
                                              iota_stor_pack_t* const pack);

extern retcode_t storage_milestone_load_by_index(storage_connection_t const* const connection, uint64_t const index,
                                                 iota_stor_pack_t* const pack);

extern retcode_t storage_milestone_load_next(storage_connection_t const* const connection, uint64_t const index,
                                             iota_stor_pack_t* const pack);

extern retcode_t storage_milestone_exist(storage_connection_t const* const connection, flex_trit_t const* const hash,
                                         bool* const exist);

extern retcode_t storage_milestone_delete(storage_connection_t const* const connection, flex_trit_t const* const hash);

/*
 * State delta operations
 */

extern retcode_t storage_state_delta_store(storage_connection_t const* const connection, uint64_t const index,
                                           state_delta_t const* const delta);

extern retcode_t storage_state_delta_load(storage_connection_t const* const connection, uint64_t const index,
                                          state_delta_t* const delta);

/*
 * Spent address operations
 */

extern retcode_t storage_spent_address_store(storage_connection_t const* const connection,
                                             flex_trit_t const* const address);

extern retcode_t storage_spent_address_exist(storage_connection_t const* const connection,
                                             flex_trit_t const* const address, bool* const exist);

extern retcode_t storage_spent_addresses_store(storage_connection_t const* const connection,
                                               hash243_set_t const addresses);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_STORAGE_H__
