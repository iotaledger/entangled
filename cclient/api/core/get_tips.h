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
#ifndef CCLIENT_API_GET_TIPS_H
#define CCLIENT_API_GET_TIPS_H

#include "cclient/http/http.h"
#include "cclient/response/get_tips.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns a list of tips (transactions not referenced by other transactions).
 *
 * @param[in] service
 * @param[out] res
 * @return #retcode_t
 */
retcode_t iota_client_get_tips(iota_client_service_t const* const service, get_tips_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_TIPS_H

/** @} */