/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/tangle/tangle.h"
#include "utils/logger_helper.h"

#define TANGLE_LOGGER_ID "consensus_tangle"

retcode_t iota_tangle_init(tangle_t *tangle, const connection_config_t *conf) {
  logger_helper_init(TANGLE_LOGGER_ID, LOGGER_INFO, true);
  return iota_stor_init(&tangle->conn, conf);
}
retcode_t iota_tangle_destroy(tangle_t *tangle) {
  logger_helper_destroy(TANGLE_LOGGER_ID);
  return iota_stor_destroy(&tangle->conn);
}

/*
 * Transaction operations
 */

retcode_t iota_tangle_transaction_store(const tangle_t *const tangle,
                                        const iota_transaction_t data_in) {
  return iota_stor_transaction_store(&tangle->conn, data_in);
}

retcode_t iota_tangle_transaction_load(const tangle_t *const tangle,
                                       const char *col_name,
                                       const trit_array_p key,
                                       iota_stor_pack_t *data_out) {
  // TODO - when more "persistance provider are implemented - should loop
  return iota_stor_transaction_load(&tangle->conn, col_name, key, data_out);
}

retcode_t iota_tangle_transaction_exist(const tangle_t *const tangle,
                                        const char *index_name,
                                        const trit_array_p key, bool *exist) {
  // TODO - when more "persistance provider are implemented - should loop
  return iota_stor_transaction_exist(&tangle->conn, index_name, key, exist);
}

retcode_t iota_tangle_transaction_update(const tangle_t *const tangle,
                                         const char *index_name,
                                         const trit_array_p key,
                                         const iota_transaction_t data_in) {
  return RC_CONSENSUS_NOT_IMPLEMENTED;
}

retcode_t iota_tangle_transaction_load_hashes(const tangle_t *const tangle,
                                              const char *index_name,
                                              const trit_array_p key,
                                              iota_stor_pack_t *pack) {
  return iota_stor_transaction_load_hashes(&tangle->conn, index_name, key,
                                           pack);
}

retcode_t iota_tangle_transaction_load_hashes_of_approvers(
    const tangle_t *const tangle, const trit_array_p approvee_hash,
    iota_stor_pack_t *pack) {
  return iota_stor_transaction_load_hashes_of_approvers(&tangle->conn,
                                                        approvee_hash, pack);
}

/*
 * Milestone operations
 */

retcode_t iota_tangle_milestone_store(const tangle_t *const tangle,
                                      const iota_milestone_t *data_in) {
  return iota_stor_milestone_store(&tangle->conn, data_in);
}

retcode_t iota_tangle_milestone_load(const tangle_t *const tangle,
                                     const char *col_name,
                                     const trit_array_p key,
                                     iota_stor_pack_t *pack) {
  return iota_stor_milestone_load(&tangle->conn, col_name, key, pack);
}

retcode_t iota_tangle_milestone_load_latest(const tangle_t *const tangle,
                                            iota_stor_pack_t *pack) {
  return iota_stor_milestone_load_latest(&tangle->conn, pack);
}

retcode_t iota_tangle_milestone_load_next(const tangle_t *const tangle,
                                          uint64_t index,
                                          iota_stor_pack_t *pack) {
  return iota_stor_milestone_load_next(&tangle->conn, index, pack);
}

retcode_t iota_tangle_milestone_exist(const tangle_t *const tangle,
                                      const char *index_name,
                                      const trit_array_p key, bool *exist) {
  return iota_stor_milestone_exist(&tangle->conn, index_name, key, exist);
}
