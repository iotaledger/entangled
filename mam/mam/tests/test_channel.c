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
#include "mam/mam/tests/test_channel_utils.h"

void test_channel(void) {
  mam_channel_t channel;
  mam_channel_t_set_t channels_1 = NULL;
  mam_channel_t_set_t channels_2 = NULL;

  mam_prng_t prng;
  tryte_t channel_name[27];
  trits_t channel_name_trits = trits_alloc(81);

  MAM_TRITS_DEF(prng_key, MAM_PRNG_SECRET_KEY_SIZE);
  prng_key = MAM_TRITS_INIT(prng_key, MAM_PRNG_SECRET_KEY_SIZE);
  trits_from_str(prng_key,
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM"
                 "NOPQRSTUVWXYZ9ABCDEFGHIJKLM");

  mam_prng_init(&prng, prng_key);

  for (size_t i = 1; i < 5; i++) {
    memset(channel_name, 'A' + 2 * i, 27);
    trytes_to_trits(channel_name, channel_name_trits.p, 27);
    TEST_ASSERT(mam_channel_create(&prng, i, channel_name_trits, &channel) == RC_OK);
    add_assign(channel.msg_ord, MAM_CHANNEL_MSG_ORD_SIZE, i);
    channel.endpoint_ord = i;
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
