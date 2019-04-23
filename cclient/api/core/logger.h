/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup cclient_core
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_API_CORE_LOGGER_H
#define CCLIENT_API_CORE_LOGGER_H

#include "utils/logger_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief logger ID
 *
 */
extern logger_id_t client_core_logger_id;

/**
 * @brief init Core API logger
 *
 */
void logger_init_client_core();

/**
 * @brief cleanup Core API logger
 *
 */
void logger_destroy_client_core();

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_CORE_LOGGER_H

/** @} */