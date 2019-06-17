/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/mam/channel.h"
#include "mam/mam/mam_channel_t_set.h"

bool mam_channel_t_set_cmp_test(mam_channel_t_set_t const channels_1, mam_channel_t_set_t const channels_2) {
  mam_channel_t_set_entry_t *entry_1 = NULL;
  mam_channel_t_set_entry_t *tmp_1 = NULL;
  mam_channel_t_set_entry_t *entry_2 = NULL;
  mam_channel_t_set_entry_t *tmp_2 = NULL;
  size_t match = 0;
  MAM_TRITS_DEF(hash, MAM_MSS_HASH_SIZE);
  hash = MAM_TRITS_INIT(hash, MAM_MSS_HASH_SIZE);
  trits_from_str(hash,
                 "ABCNKOZWYSDF9OABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9QABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9CABCNKOZWYSDF9");

  if (mam_channel_t_set_size(channels_1) != mam_channel_t_set_size(channels_2)) {
    return false;
  }

  HASH_ITER(hh, channels_1, entry_1, tmp_1) {
    HASH_ITER(hh, channels_2, entry_2, tmp_2) {
      if (memcmp(entry_1->value.mss.root, entry_2->value.mss.root, MAM_ENDPOINT_ID_TRIT_SIZE) == 0) {
        if (!trits_cmp_eq(entry_1->value.name_size, entry_2->value.name_size)) {
          continue;
        }
        if (!trits_cmp_eq(entry_1->value.name, entry_2->value.name)) {
          continue;
        }
        if (memcmp(entry_1->value.msg_ord, entry_2->value.msg_ord, MAM_CHANNEL_MSG_ORD_SIZE) != 0) {
          continue;
        }
        if (entry_1->value.endpoint_ord != entry_2->value.endpoint_ord) {
          continue;
        }
        MAM_TRITS_DEF(sig1, MAM_MSS_SIG_SIZE(entry_1->value.mss.height));
        MAM_TRITS_DEF(sig2, MAM_MSS_SIG_SIZE(entry_2->value.mss.height));
        sig1 = MAM_TRITS_INIT(sig1, MAM_MSS_SIG_SIZE(entry_1->value.mss.height));
        sig2 = MAM_TRITS_INIT(sig2, MAM_MSS_SIG_SIZE(entry_2->value.mss.height));
        if (mam_mss_sign(&entry_1->value.mss, hash, sig1) != RC_OK) {
          continue;
        }
        if (mam_mss_sign(&entry_2->value.mss, hash, sig2) != RC_OK) {
          continue;
        }
        if (trits_cmp_eq(sig1, sig2)) {
          match++;
          break;
        }
        // TODO check endpoints
      }
    }
  }

  return match == mam_channel_t_set_size(channels_1);
}
