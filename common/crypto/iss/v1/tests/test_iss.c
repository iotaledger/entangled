/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include <unity/unity.h>

#include "common/crypto/iss/normalize.h"
#include "common/crypto/iss/v1/iss.h"
#include "common/crypto/kerl/kerl.h"

static tryte_t *seed = (tryte_t *)"NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN";
static tryte_t *message = (tryte_t *)
    "JCRNMXX9DIEVJJG9VW9QDUMVDGDVHANQDTCPPOPHLTBUBXULSIALRBVUINDPNGUFZLKDPOK9WBJMYCXF9"
    "MFQN9ZKMROOXHULIDDXRNWMDENBWJWVVA9XPNHQUVDFSMQ9ETWKWGLOLYPWW9GQPVNDYJIRDBWVCBUHUE"
    "GELSTLEXGAMMQAHSUEABKUSFOVGYRQBXJMORXIDTIPENPAFIUV9DOGZCAEPRJQOISRZDZBWWQQJVQDS9Y"
    "GCMNADNVSUTXXAONPHBFCMWSVFYYXXWDZXFP9SZGLRCHHGKLNAQPMAXHFUUSQEKDAPH9GFVHMYDITCTFS"
    "IJEZFADOJVDOEXOTDDPZYLKKDHCGPXYMGRKAGOEQYHTCTGKMZOKMZJLCQOYE9KFVRQLXDPBALUSEQSQDF"
    "PPUYALCDYWSHANNQYKIMAZMKQQ9XVCSJHAWXLY9IIREZTSOFRMRGKDQPIEMDXTBDTY9DKOAIUEGNLUSRF"
    "ZYPRNUOHFGDYIWFVKIUNYBGBHICRQTLDQQUTJX9DDSQANVKMCDZ9VEQBCHHSATVFIDYR9XUSDJHQDRBVK"
    "9JUUZVWGCCWVXAC9ZIOKBWOKCTCJVXIJFBSTLNZCPJMAKDPYLTHMOKLFDNONJLLDBDXNFKPKUBKDU9QFS"
    "XGVXS9PEDBDDBGFESSKCWUWMTOGHDLOPRILYYPSAQVTSQYLIPK9ATVMMYSTASHEZEFWBUNR9XKGCHR9MB";

void signature_resolves_to_address_test(void) {
  int index = 10;
  int nof = 1;
  trit_t seed_trits[HASH_LENGTH_TRIT];
  trit_t subseed_trits[HASH_LENGTH_TRIT];
  trit_t key[nof * ISS_KEY_LENGTH];
  trit_t signature[nof * ISS_KEY_LENGTH];
  trit_t key_digest[nof * HASH_LENGTH_TRIT];
  trit_t sig_digest[HASH_LENGTH_TRIT];
  size_t message_length = strlen((char *)message);
  trit_t message_trits[3 * message_length];
  trit_t message_hash[HASH_LENGTH_TRIT];
  sponge_type_t sponge_types[2] = {SPONGE_CURLP81, SPONGE_KERL};

  trytes_to_trits(seed, seed_trits, HASH_LENGTH_TRYTE);

  for (int i = 0; i < 2; i++) {
    sponge_t sponge;
    Kerl kerl;

    kerl_init(&kerl);
    sponge_init(&sponge, sponge_types[i]);
    iss_subseed(&sponge, seed_trits, subseed_trits, index);
    iss_key(&sponge, subseed_trits, key, nof * ISS_KEY_LENGTH);
    trytes_to_trits(message, message_trits, message_length);
    kerl_absorb(&kerl, message_trits, 3 * message_length);
    kerl_squeeze(&kerl, message_hash, HASH_LENGTH_TRIT);
    normalize_hash_to_trits(message_hash, message_hash);
    iss_signature(&sponge, signature, message_hash, key, nof * ISS_KEY_LENGTH);
    iss_sig_digest(&sponge, sig_digest, message_hash, signature, nof * ISS_KEY_LENGTH);
    iss_address(&sponge, sig_digest, sig_digest, HASH_LENGTH_TRIT);
    iss_key_digest(&sponge, key, key_digest, nof * ISS_KEY_LENGTH);
    iss_address(&sponge, key_digest, key_digest, nof * HASH_LENGTH_TRIT);
    TEST_ASSERT_EQUAL_MEMORY(key_digest, sig_digest, HASH_LENGTH_TRIT);
    sponge_destroy(&sponge);
  }
}

void address_generation_test(void) {
  int index = 0;
  int nof = 2;
  trit_t seed_trits[HASH_LENGTH_TRIT];
  trit_t subseed_trits[HASH_LENGTH_TRIT];
  trit_t key[nof * ISS_KEY_LENGTH];
  trit_t digest[nof * HASH_LENGTH_TRIT];
  tryte_t address[HASH_LENGTH_TRYTE];
  sponge_type_t sponge_types[2] = {SPONGE_CURLP81, SPONGE_KERL};
  tryte_t *hashes[2] = {(tryte_t *)"D9XCNSCCAJGLWSQOQAQNFWANPYKYMCQ9VCOMROLDVLONPPLDFVPIZNAPVZLQMPFYJPAHUKIAEKNCQIYJZ",
                        (tryte_t *)"MDWYEJJHJDIUVPKDY9EACGDJUOP9TLYDWETUBOYCBLYXYYYJYUXYUTCTPTDGJYFKMQMCNZDQPTBE9AFIW"};

  trytes_to_trits(seed, seed_trits, HASH_LENGTH_TRYTE);

  for (int i = 0; i < 2; i++) {
    sponge_t sponge;

    sponge_init(&sponge, sponge_types[i]);
    iss_subseed(&sponge, seed_trits, subseed_trits, index);
    iss_key(&sponge, subseed_trits, key, nof * ISS_KEY_LENGTH);
    iss_key_digest(&sponge, key, digest, nof * ISS_KEY_LENGTH);
    iss_address(&sponge, digest, digest, nof * HASH_LENGTH_TRIT);
    trits_to_trytes(digest, address, HASH_LENGTH_TRIT);
    TEST_ASSERT_EQUAL_MEMORY(hashes[i], address, HASH_LENGTH_TRYTE);
    sponge_destroy(&sponge);
  }
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(signature_resolves_to_address_test);
  RUN_TEST(address_generation_test);

  return UNITY_END();
}
