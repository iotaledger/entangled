/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup cclient_extended
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_API_EXTENDED_LOGGER_H
#define CCLIENT_API_EXTENDED_LOGGER_H

#include "utils/logger_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Logger ID for extended APIs.
 *
 */
extern logger_id_t client_extended_logger_id;

/**
 * @brief Init extended API logger
 *
 */
void logger_init_client_extended();

/**
 * @brief Destroy extended API logger
 *
 */
void logger_destroy_client_extended();

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_EXTENDED_LOGGER_H

/** @} */