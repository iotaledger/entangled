/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_TEST_UTILS_TANGLE_H__
#define __CONSENSUS_TEST_UTILS_TANGLE_H__

#include "common/errors.h"
#include "common/storage/connection.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t tangle_setup(tangle_t *const tangle, connection_config_t *const config,
                       char *test_db_path, char *ciri_db_path);

retcode_t tangle_cleanup(tangle_t *const tangle, char *test_db_path);

#ifdef __cplusplus
}
#endif

#endif //__CONSENSUS_TEST_UTILS_TANGLE_H__
