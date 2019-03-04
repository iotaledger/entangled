/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include <unity/unity.h>

#include "common/trinary/trit_tryte.h"
#include "mam/mam/channel.h"
#include "mam/mam/mam_channel_t_set.h"
#include "mam/mam/message.h"
#include "mam/test_utils/test_utils.h"

static bool mam_channel_t_set_cmp_test(mam_channel_t_set_t const channels_1,
                                       mam_channel_t_set_t const channels_2) {
  mam_channel_t_set_entry_t *entry_1 = NULL;
  mam_channel_t_set_entry_t *tmp_1 = NULL;
  mam_channel_t_set_entry_t *entry_2 = NULL;
  mam_channel_t_set_entry_t *tmp_2 = NULL;
  size_t match = 0;
  MAM_TRITS_DEF0(hash, MAM_MSS_HASH_SIZE);
  hash = MAM_TRITS_INIT(hash, MAM_MSS_HASH_SIZE);
  trits_from_str(hash,
                 "ABCNKOZWYSDF9OABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9QABCNKOZWYSDF9"
                 "ABCNKOZWYSDF9CABCNKOZWYSDF9");

  if (mam_channel_t_set_size(channels_1) !=
      mam_channel_t_set_size(channels_2)) {
    return false;
  }

  HASH_ITER(hh, channels_1, entry_1, tmp_1) {
    HASH_ITER(hh, channels_2, entry_2, tmp_2) {
      if (memcmp(entry_1->value.mss.root, entry_2->value.mss.root,
                 MAM_ENDPOINT_ID_SIZE) == 0 &&
          trits_cmp_eq(entry_1->value.name, entry_2->value.name) &&
          memcmp(entry_1->value.msg_ord, entry_2->value.msg_ord,
                 MAM_CHANNEL_MSG_ORD_SIZE) == 0) {
        MAM_TRITS_DEF0(sig1, MAM_MSS_SIG_SIZE(entry_1->value.mss.height));
        MAM_TRITS_DEF0(sig2, MAM_MSS_SIG_SIZE(entry_2->value.mss.height));
        sig1 =
            MAM_TRITS_INIT(sig1, MAM_MSS_SIG_SIZE(entry_1->value.mss.height));
        sig2 =
            MAM_TRITS_INIT(sig2, MAM_MSS_SIG_SIZE(entry_2->value.mss.height));
        mam_mss_sign(&entry_1->value.mss, hash, sig1);
        mam_mss_sign(&entry_2->value.mss, hash, sig2);
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

void test_channel(void) {
  mam_channel_t channel;
  mam_channel_t_set_t channels_1 = NULL;
  mam_channel_t_set_t channels_2 = NULL;

  mam_prng_t prng;
  tryte_t channel_name[27];
  trits_t channel_name_trits = trits_alloc(81);

  MAM_TRITS_DEF0(prng_key, MAM_PRNG_KEY_SIZE);
  prng_key = MAM_TRITS_INIT(prng_key, MAM_PRNG_KEY_SIZE);
  trits_from_str(prng_key,
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM");

  mam_prng_init(&prng, prng_key);

  for (size_t i = 1; i < 5; i++) {
    memset(channel_name, 'A' + 2 * i, 27);
    trytes_to_trits(channel_name, channel_name_trits.p, 27);
    TEST_ASSERT(mam_channel_create(&prng, i, channel_name_trits, &channel) ==
                RC_OK);
    add_assign(channel.msg_ord, MAM_CHANNEL_MSG_ORD_SIZE, i);
    TEST_ASSERT(mam_channel_t_set_add(&channels_1, &channel) == RC_OK);
  }

  size_t size = mam_channels_serialized_size(channels_1);

  trits_t trits = trits_alloc(size);
  trits_t cpy = trits;

  mam_channels_serialize(channels_1, &trits);

  TEST_ASSERT(mam_channels_deserialize(&cpy, &prng, &channels_2) == RC_OK);

  TEST_ASSERT_TRUE(mam_channel_t_set_cmp_test(channels_1, channels_2));

  mam_channels_destroy(&channels_1);
  mam_channels_destroy(&channels_2);
  trits_free(trits);
  trits_free(channel_name_trits);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_channel);

  return UNITY_END();
}
