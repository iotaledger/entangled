/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/wallet/wallet.h"
#include "mam/v2/mam/mam_pre_shared_key_t_set.h"

retcode_t mam_wallet_init(mam_wallet_t* const wallet) {
  wallet->psks = NULL;

  return RC_OK;
}

retcode_t mam_wallet_destroy(mam_wallet_t* const wallet) {
  mam_pre_shared_key_t_set_free(&wallet->psks);

  return RC_OK;
}
