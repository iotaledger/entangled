/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "mam/v2/tests/common.h"
#include "utils/macros.h"

void test_sponge_hash(trit_array_t const *const input) {
  test_sponge_t _s;
  sponge_t *s = test_sponge_init(&_s);
  tryte_t *expected_hash_trytes = (tryte_t *)
      "Z9DDCLPZASLK9BCLPZASLKDVICXESYBIWBHJHQYOKIHNXHZDQVCFGDVIUTDVISKTMDG9EMON"
      "OYXPODPWU";
  size_t expected_hash_length = strlen((char *)expected_hash_trytes);
  TRIT_ARRAY_MAKE_FROM_TRYTES(expected_hash, expected_hash_length,
                              expected_hash_trytes);
  TRIT_ARRAY_DECLARE(output, 243);

  sponge_init(s);
  sponge_absorb_flex(s, MAM2_SPONGE_CTL_DATA, input);
  sponge_squeeze_flex(s, MAM2_SPONGE_CTL_HASH, &output);

  TEST_ASSERT(trit_array_is_equal(&expected_hash, &output));
}

void test_sponge_encr(trit_array_t const *const key,
                      trit_array_t const *const input) {
  test_sponge_t _s;
  sponge_t *s = test_sponge_init(&_s);
  tryte_t *expected_encr_trytes = (tryte_t *)
    "ABEZQN99JVWYZAZONRCHKUNKUSKSKSKTMDGQN99JVWYZAZONRCHKUNTYKUNKUSKTMDGQN99J"
    "VWYZAZONRNDAAZNODFGABCKNLMOXYWERXCVHAZNODFGXCVKNLMOXHABCYZ99WERFGWERVHAB"
    "CKXCNLM9AZNODOXYZ9";
  size_t expected_encr_length = strlen((char *)expected_encr_trytes);
  TRIT_ARRAY_MAKE_FROM_TRYTES(expected_encr, expected_encr_length,
                              expected_encr_trytes);
  TRIT_ARRAY_DECLARE(output, 486);

  sponge_init(s);
  sponge_absorb_flex(s, MAM2_SPONGE_CTL_KEY, key);
  sponge_encr_flex(s, input, &output);

  TEST_ASSERT(trit_array_is_equal(&expected_encr, &output));
}

void test_sponge_decr(trit_array_t const *const key,
                      trit_array_t const *const input) {
  test_sponge_t _s;
  sponge_t *s = test_sponge_init(&_s);
  tryte_t *expected_decr_trytes = (tryte_t *)
      "ABSNTANLTJBBAAZPPERLQAVSLPEKVPEFUBITANLTJBBAAZPPERLQAVAPQAVSLPEFUBITANLT"
      "JBBAAZPPEKWZAZNODFGABCKNLMOXYWERXCVHAZNODFGXCVKNLMOXHABCYZ99WERFGWERVHAB"
      "CKXCNLM9AZNODOXYZ9";
  size_t expected_decr_length = strlen((char *)expected_decr_trytes);
  TRIT_ARRAY_MAKE_FROM_TRYTES(expected_decr, expected_decr_length,
                              expected_decr_trytes);
  TRIT_ARRAY_DECLARE(output, 486);

  sponge_init(s);
  sponge_absorb_flex(s, MAM2_SPONGE_CTL_KEY, key);
  sponge_decr_flex(s, &output, input);

  TEST_ASSERT(trit_array_is_equal(&expected_decr, &output));
}

void test_prng_gen(trit_array_t const *const key,
                   trit_array_t const *const nonce) {
  test_sponge_t _s;
  test_prng_t _p;
  sponge_t *s = test_sponge_init(&_s);
  prng_t *p = test_prng_init(&_p, s);
  tryte_t *expected_gen_trytes = (tryte_t *)
      "99MGDGQN99JVWYZZZNODFGWERXCVHXCVHABCKNLMOXYZ99AZNODFGWERFGWERXCVHABCKNLM"
      "OXYZ99AZNOD99ABCKNLMOXYZA99999999999999999999999999999999999999999999999"
      "999999999999999999";
  size_t expected_gen_length = strlen((char *)expected_gen_trytes);
  TRIT_ARRAY_MAKE_FROM_TRYTES(expected_gen, expected_gen_length,
                              expected_gen_trytes);
  TRIT_ARRAY_DECLARE(output, 486);

  prng_init(p, s, key->trits);
  prng_gen(p, 0, nonce, &output);

  TEST_ASSERT(trit_array_is_equal(&expected_gen, &output));
}

void test_wots_gen_sign(trit_array_t const *const key,
                        trit_array_t const *const nonce,
                        trit_array_t const *const hash) {
  test_sponge_t _s;
  test_prng_t _p;
  test_wots_t _w;
  sponge_t *s = test_sponge_init(&_s);
  prng_t *p = test_prng_init(&_p, s);
  wots_t *w = test_wots_init(&_w, s);

  TRIT_ARRAY_DECLARE(pk, MAM2_WOTS_PK_SIZE);
  TRIT_ARRAY_DECLARE(sig, MAM2_WOTS_SIG_SIZE);

  prng_init(p, s, key->trits);
  wots_gen_sk(w, p, nonce);
  wots_calc_pk(w, &pk);
  wots_sign(w, hash, &sig);
  // TODO - test/fix wots_gen_sign_sig
}

void test() {
  tryte_t *nonce_trytes = (tryte_t *)"9ABCKNLMOXYZ";
  size_t nonce_length = strlen((char *)nonce_trytes);
  TRIT_ARRAY_MAKE_FROM_TRYTES(nonce, nonce_length, nonce_trytes);

  tryte_t *key_trytes = (tryte_t *)
      "ABCKNLMOXYZ99AZNODFGWERXCVHXCVHABCKNLMOXYZ99AZNODFGWERFGWERXCVHABCKNLMOX"
      "YZ99AZNOD";
  size_t key_length = strlen((char *)key_trytes);
  TRIT_ARRAY_MAKE_FROM_TRYTES(key, key_length, key_trytes);

  tryte_t *hash_trytes = (tryte_t*)
      "BACKNLMOXYZ99AZNODFGWERXCVHXCVHABCKNLMOXYZ99AZNODFGWERFGWERXCVHABCKNLMOX"
      "YZ99AZ";
  size_t hash_length = strlen((char *)hash_trytes);
  TRIT_ARRAY_MAKE_FROM_TRYTES(hash, hash_length, hash_trytes);

  tryte_t *input_trytes = (tryte_t*)
      "ABCKNLMOXYZ99AZNODFGWERXCVHXCVHABCKNLMOXYZ99AZNODFGWERFGWERXCVHABCKNLMOX"
      "YZ99AZNODZ99AZNODFGABCKNLMOXYWERXCVHAZNODFGXCVKNLMOXHABCYZ99WERFGWERVHAB"
      "CKXCNLM9AZNODOXYZ9";
  size_t input_length = strlen((char *)input_trytes);
  TRIT_ARRAY_MAKE_FROM_TRYTES(input, input_length, input_trytes);

  test_sponge_hash(&input);
  // FIXME (@tsvisabo) test dec(enc(X)) = X
  test_sponge_encr(&key, &input);
  test_sponge_decr(&key, &input);
  test_prng_gen(&key, &nonce);
  test_wots_gen_sign(&key, &nonce, &hash);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test);

  return UNITY_END();
}
