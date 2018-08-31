/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_TEST_DEFINITIONS_DEFS_H__
#define __CONSENSUS_TEST_DEFINITIONS_DEFS_H__

#include "common/errors.h"
#include "common/storage/connection.h"

#ifdef __cplusplus
extern "C" {
#endif

// void get_test_db_path(char *path);
// void get_src_db_path(char *path);

// void get_db_path(bool debug_mode);

retcode_t test_setup(tangle_t *const tangle, connection_config_t *const config);
retcode_t test_cleanup(tangle_t *const tangle);

#ifdef __cplusplus
}
#endif

#endif
