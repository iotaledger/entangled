/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 [ITSec Lab]

 *
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file curl.h
\brief Curl implementation.
*/
#pragma once

#include "mam/v2/defs.h"
#include "mam/v2/sponge.h"
#include "trits.h"

/*! \brief Curl state. */
typedef struct _curl_sponge {
  isponge i;         /*!< sponge interface */
  sponge_state_t s;  /*!< state */
  sponge_state_t s2; /*!< additional memory */
} curl_sponge;

#define MAM2_CURL_RATE 243

/*! \brief Curl sponge interface initialization. */
isponge *curl_sponge_init(curl_sponge *c);

void curl_init(curl_sponge *s);
void curl_absorb(curl_sponge *s, trits_t X);
void curl_squeeze(curl_sponge *s, trits_t Y);
