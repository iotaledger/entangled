/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CONSENSUS_TEST_UTILS_SPENT_ADDRESSES_H__
#define __CIRI_CONSENSUS_TEST_UTILS_SPENT_ADDRESSES_H__

#include "ciri/consensus/spent_addresses/spent_addresses_provider.h"
#include "common/errors.h"
#include "common/storage/connection.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t spent_addresses_setup(spent_addresses_provider_t *const sap, storage_connection_config_t *const config,
                                char *test_db_path);

retcode_t spent_addresses_cleanup(spent_addresses_provider_t *const sap, char *test_db_path);

#ifdef __cplusplus
}
#endif

#endif  //__CIRI_CONSENSUS_TEST_UTILS_SPENT_ADDRESSES_H__
