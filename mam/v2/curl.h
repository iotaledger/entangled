/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_CURL_H__
#define __MAM_V2_CURL_H__

#include "mam/v2/sponge.h"
#include "mam/v2/trits.h"

/*! \brief Curl state. */
typedef struct _curl_sponge {
  sponge_t i;        /*!< sponge interface */
  sponge_state_t s;  /*!< state */
  sponge_state_t s2; /*!< additional memory */
} curl_sponge;

#define MAM2_CURL_RATE 243

/*! \brief Curl sponge interface initialization. */
sponge_t *curl_sponge_init(curl_sponge *c);

void curl_init(curl_sponge *s);
void curl_absorb(curl_sponge *s, trits_t X);
void curl_squeeze(curl_sponge *s, trits_t Y);

#endif  // __MAM_V2_CURL_H__
