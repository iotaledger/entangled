/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_GET_TIPS_H
#define CCLIENT_API_GET_TIPS_H

#include "cclient/http/http.h"
#include "cclient/response/get_tips.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns the list of tips.
 *
 * https://iota.readme.io/reference#gettips
 *
 * @param service IRI node end point.
 * @param res Response containing the tips
 *
 * @return error value.
 */
retcode_t iota_client_get_tips(const iota_client_service_t* const service, get_tips_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_TIPS_H
