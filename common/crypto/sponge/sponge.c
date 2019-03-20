/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/crypto/curl-p/trit.h"
#include "common/crypto/kerl/kerl.h"
#include "common/crypto/sponge/sponge.h"

retcode_t sponge_init(sponge_t* const sponge, sponge_type_t const type) {
  if (sponge == NULL) {
    return RC_NULL_PARAM;
  }

  if (type == SPONGE_CURLP27) {
    if ((sponge->state = malloc(sizeof(Curl))) == NULL) {
      return RC_OOM;
    }
    ((Curl*)(sponge->state))->type = CURL_P_27;
    sponge->absorb = (sponge_absorb_t)curl_absorb;
    sponge->squeeze = (sponge_squeeze_t)curl_squeeze;
    sponge->reset = (sponge_reset_t)curl_reset;
    curl_init((Curl*)sponge->state);
  } else if (type == SPONGE_CURLP81) {
    if ((sponge->state = malloc(sizeof(Curl))) == NULL) {
      return RC_OOM;
    }
    ((Curl*)(sponge->state))->type = CURL_P_81;
    sponge->absorb = (sponge_absorb_t)curl_absorb;
    sponge->squeeze = (sponge_squeeze_t)curl_squeeze;
    sponge->reset = (sponge_reset_t)curl_reset;
    curl_init((Curl*)sponge->state);
  } else if (type == SPONGE_KERL) {
    if ((sponge->state = malloc(sizeof(Kerl))) == NULL) {
      return RC_OOM;
    }
    sponge->absorb = (sponge_absorb_t)kerl_absorb;
    sponge->squeeze = (sponge_squeeze_t)kerl_squeeze;
    sponge->reset = (sponge_reset_t)kerl_reset;
    kerl_init((Kerl*)sponge->state);
  } else if (type == SPONGE_TROIKA) {
  } else {
    return RC_CRYPTO_UNSUPPORTED_SPONGE_TYPE;
  }

  return RC_OK;
}

retcode_t sponge_absorb(sponge_t* const sponge, trit_t const* const input, size_t const length) {
  if (sponge == NULL) {
    return RC_NULL_PARAM;
  }

  sponge->absorb(sponge->state, input, length);

  return RC_OK;
}

retcode_t sponge_squeeze(sponge_t* const sponge, trit_t* const output, size_t const length) {
  if (sponge == NULL) {
    return RC_NULL_PARAM;
  }

  sponge->squeeze(sponge->state, output, length);

  return RC_OK;
}

retcode_t sponge_reset(sponge_t* const sponge) {
  if (sponge == NULL) {
    return RC_NULL_PARAM;
  }

  sponge->reset(sponge->state);

  return RC_OK;
}

retcode_t sponge_destroy(sponge_t* const sponge) {
  if (sponge == NULL) {
    return RC_NULL_PARAM;
  }

  free(sponge->state);
  sponge->state = NULL;

  return RC_OK;
}
