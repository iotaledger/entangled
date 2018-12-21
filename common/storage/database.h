/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_STORAGE_DATABASE_H__
#define __COMMON_STORAGE_DATABASE_H__

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the database backend
 * Should only be called once per process
 *
 * @return a status code
 */
retcode_t database_init();

/**
 * Destroys the database backend
 * Should only be called once per process
 *
 * @return a status code
 */
retcode_t database_destroy();

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_STORAGE_DATABASE_H__
