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

#include "mam/v2/ntru/ntru.h"
#include "mam/v2/ntru/poly.h"
#include "mam/v2/test_utils/test_utils.h"

/* \brief This special trivial spongos transform is used
in order to get `AE(spongos;key)=1^n`. */
static void ntru_test_f(void *buf, trit_t *s) {
  trits_t *b = (trits_t *)buf;
  trits_t x = trits_from_rep(MAM2_SPONGE_RATE, s);
  MAM2_ASSERT(0 != buf);
  trits_add(x, trits_advance(b, MAM2_SPONGE_RATE), x);
}

static void ntru_test_gen(ntru_t *n, trits_t rf, trits_t rg, trits_t pk) {
  poly_coeff_t *f;
  MAM2_POLY_DEF(g);
  MAM2_POLY_DEF(h);

  f = (poly_coeff_t *)n->f;
  poly_small_from_trits(f, rf);
  poly_small_from_trits(g, rg);

  /* f := NTT(1+3f) */
  poly_small_mul3(f, f);
  poly_small3_add1(f);
  poly_ntt(f, f);

  /* g := NTT(3g) */
  poly_small_mul3(g, g);
  poly_ntt(g, g);

  MAM2_ASSERT(poly_has_inv(f) && poly_has_inv(g));

  /* h := NTT(3g/(1+3f)) */
  poly_inv(f, h);
  poly_conv(g, h, h);
  poly_intt(h, h);

  poly_to_trits(h, pk);
}

static void ntru_decr_fail_test(void) {
  test_sponge_t test_sponge;
  test_spongos_t test_spongos;
  test_prng_t test_prng;
  test_ntru_t test_ntru;

  sponge_t *sponge = test_sponge_init(&test_sponge);
  spongos_t *spongos = test_spongos_init(&test_spongos, sponge);
  prng_t *prng = test_prng_init(&test_prng, sponge);
  ntru_t *ntru = test_ntru_init(&test_ntru);

  poly_coeff_t *f0;
  MAM2_TRITS_DEF0(nonce, 3 * 10);
  MAM2_TRITS_DEF0(pk, MAM2_NTRU_PK_SIZE);
  MAM2_TRITS_DEF0(key, MAM2_NTRU_KEY_SIZE);
  MAM2_TRITS_DEF0(ekey, MAM2_NTRU_EKEY_SIZE);
  MAM2_TRITS_DEF0(dekey, MAM2_NTRU_KEY_SIZE);
  MAM2_TRITS_DEF0(u, MAM2_NTRU_SK_SIZE);
  MAM2_TRITS_DEF0(v, MAM2_NTRU_SK_SIZE);
  trit_t state[MAM2_SPONGE_WIDTH];
  MAM2_TRITS_DEF0(buf, MAM2_NTRU_EKEY_SIZE + MAM2_NTRU_SK_SIZE);
  sponge_t _s[1];
  spongos_t s[1];
  /* mask is used to make spongos output equal 1* */
  trits_t mask;

  nonce = MAM2_TRITS_INIT(nonce, 3 * 10);
  pk = MAM2_TRITS_INIT(pk, MAM2_NTRU_PK_SIZE);
  key = MAM2_TRITS_INIT(key, MAM2_NTRU_KEY_SIZE);
  ekey = MAM2_TRITS_INIT(ekey, MAM2_NTRU_EKEY_SIZE);
  dekey = MAM2_TRITS_INIT(dekey, MAM2_NTRU_KEY_SIZE);
  f0 = (poly_coeff_t *)ntru->f;
  u = MAM2_TRITS_INIT(u, MAM2_NTRU_SK_SIZE);
  v = MAM2_TRITS_INIT(v, MAM2_NTRU_SK_SIZE);
  buf = MAM2_TRITS_INIT(buf, MAM2_NTRU_EKEY_SIZE + MAM2_NTRU_SK_SIZE);
  trits_set_zero(buf);

  /**
   * Decryption failure happens when |3ug+t+3tf|=6*1024+1 (as a certain
   * coefficient would be overflown mods q) where f&g - private key,
   * pk=3g/(1+3f) - public key, u - one-time secret, t = AE(u*pk;key) - noise /
   * encrypted key. Taking f=g=1+x+..+x^511-x^512-x^513-..-x^1023 and
   * t=u=1+x+..+x^1023 results in decryption failure: tf=ug = ..+1024x^511 (mod
   * 1+x^1024) => 3ug+t+3tf = ..+(6*1024+1)x^511 = ..-6*1024x^511 (mods q)!
   * 6*1024+1 = 1 (mods 3) != 0 (mods 3) = -(6*1024).
   */

  trits_set1(v, 1);
  trits_set1(trits_drop(v, 512), -1);
  ntru_test_gen(ntru, v, v, pk);

  /* use a special transform for this case */
  s->sponge = _s;
  s->sponge->f = ntru_test_f;
  s->sponge->stack = &mask;
  s->sponge->s = state;

  trits_set1(u, 1);
  trits_set1(key, 0);
  {
    /* mask is chosen in such a way that AE(u*pk;key)=1* and MAC(u*pk;key)=1* */
    trits_set_zero(buf);
    trits_from_str(trits_drop(buf, 8748),
                   "BFFMMQLSIEVFEYRWMBR9LEIHHNE"
                   "DSKDYFXEY9XLRWGCDSLLGEPWYER"
                   "FUISUGDMXSJKROHUVMPVQXIKHNH"
                   "WJWGKFA9X9MWZGXTKQFRYHIIDYQ"
                   "LGLXVEWFJHWLTMMTIQICXOYSRKW"
                   "OHKKQYOHFJCFXVQTBFS9LLYEUHH"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMA");

    {
      /* encrypt with (1+x+x^2+..) as noise */
      /* key=0^n so that AE(u*pk;key)=1^n */
      mask = buf;
      ntru_encr_r(pk, s, u, key, ekey);

      /* decryption should fail! as there will be overflow */
      mask = buf;
      TEST_ASSERT_TRUE(!ntru_decr(ntru, s, ekey, dekey));
      /* failure is detected by ntru_decr by checking MAC tag */
    }

    {
      /* modify key so that t=AE(u*pk;key)=01* */
      trits_put1(key, 1);
      mask = buf;
      ntru_encr_r(pk, s, u, key, ekey);
      mask = buf;
      TEST_ASSERT_TRUE(ntru_decr(ntru, s, ekey, dekey));
      trits_put1(key, 0);
    }
  }

  {
    trits_set_zero(buf);
    /* mask such that AE(u*pk;key)=1* */
    trits_from_str(trits_drop(buf, 8748),
                   "MMMYGINZFAULEAENTHMQWXCQLXX"
                   "SOYZFXBLHCPIIYKOSEUUEASLLUM"
                   "IUBSSU9ZQPHJBDMWWUUOWZGEOFU"
                   "TDJKJQVQJKXYCNFOCBERBFPGUUK"
                   "X9WYUXYBKTHHEOLUMWGXMCSLUOM"
                   "TOSSZHU9QEYFWNENFIUGMHDFAPB"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMA");
    /* modify u = 01* */
    trits_put1(u, 0);
    mask = buf;
    ntru_encr_r(pk, s, u, key, ekey);
    mask = buf;
    TEST_ASSERT_TRUE(ntru_decr(ntru, s, ekey, dekey));
    trits_put1(u, 1);
  }

  {
    trits_set_zero(buf);
    trits_from_str(trits_drop(buf, 8748),
                   "LNSBUQPMBYMXXAXWQRTHKHGUZZY"
                   "NEFSHEXCSWKQC9GYIIIVXQXMNBR"
                   "KQMULLNXFFUHRUWACFPXMHDHWLY"
                   "CTXRFFQHEO9Q9FJYHBGWHRTF9HH"
                   "QJYYMJEXUQDLUCSYWSPJULWMVOG"
                   "UMFRCYSEENGGFIBJGHQXLWNRYSI"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMMMMMMMMMMM"
                   "MMMMMMMMMMMMMMMMMA");
    /* modify f = 01* */
    trits_copy(v, u);
    trits_put1(trits_drop(v, 511), 0);
    ntru_test_gen(ntru, u, v, pk);
    trits_set1(u, 1);
    trits_put1(u, 0);
    mask = buf;
    ntru_encr_r(pk, s, u, key, ekey);
    mask = buf;
    TEST_ASSERT_TRUE(ntru_decr(ntru, s, ekey, dekey));
    trits_put1(u, 1);
  }
}

static void ntru_test(void) {
  test_sponge_t test_sponge;
  test_spongos_t test_spongos;
  test_prng_t test_prng;
  test_ntru_t test_ntru;

  sponge_t *sponge = test_sponge_init(&test_sponge);
  spongos_t *spongos = test_spongos_init(&test_spongos, sponge);
  prng_t *prng = test_prng_init(&test_prng, sponge);
  ntru_t *ntru = test_ntru_init(&test_ntru);

  size_t i;
  size_t test_count = 30;
  poly_coeff_t *f0;
  MAM2_POLY_DEF(f);
  MAM2_TRITS_DEF0(nonce, 3 * 10);
  MAM2_TRITS_DEF0(pk, MAM2_NTRU_PK_SIZE);
  MAM2_TRITS_DEF0(key, MAM2_NTRU_KEY_SIZE);
  MAM2_TRITS_DEF0(ekey, MAM2_NTRU_EKEY_SIZE);
  MAM2_TRITS_DEF0(dekey, MAM2_NTRU_KEY_SIZE);
  nonce = MAM2_TRITS_INIT(nonce, 3 * 10);
  pk = MAM2_TRITS_INIT(pk, MAM2_NTRU_PK_SIZE);
  key = MAM2_TRITS_INIT(key, MAM2_NTRU_KEY_SIZE);
  ekey = MAM2_TRITS_INIT(ekey, MAM2_NTRU_EKEY_SIZE);
  dekey = MAM2_TRITS_INIT(dekey, MAM2_NTRU_KEY_SIZE);
  f0 = (poly_coeff_t *)ntru->f;

  trits_from_str(key,
                 "AAABBBCCCAAABBBCCCAAABBBCCC"
                 "AAABBBCCCAAABBBCCCAAABBBCCC"
                 "AAABBBCCCAAABBBCCCAAABBBCCC");
  /* it'spongos safe to reuse sponge from spongos for prng */
  /* as spongos is exclusively used in ntru_encr/ntru_decr. */
  prng_init(prng, spongos->sponge, key);

  i = 0;
  trits_set_zero(key);

  do {
    TEST_ASSERT_TRUE(trits_from_str(nonce, "NONCE9PK99"));
    trits_set_zero(pk);
    trits_put1(pk, 1);
    poly_small_from_trits(f, trits_take(pk, MAM2_NTRU_SK_SIZE));
    ntru_gen(ntru, prng, nonce, pk);
    poly_add(f, f0, f);

    do {
      TEST_ASSERT_TRUE(trits_from_str(nonce, "NONCE9ENC9"));
      ntru_encr(pk, prng, spongos, key, nonce, ekey);

      TEST_ASSERT_TRUE(ntru_decr(ntru, spongos, ekey, dekey));
      TEST_ASSERT_TRUE(trits_cmp_eq(key, dekey));

      trits_put1(ekey, trit_add(trits_get1(ekey), 1));
      TEST_ASSERT_TRUE(!ntru_decr(ntru, spongos, ekey, dekey));
      TEST_ASSERT_TRUE(!trits_cmp_eq(key, dekey));
      trits_put1(ekey, trit_sub(trits_get1(ekey), 1));

      /*trits_put1(ntru_sk_trits(n), trit_add(trits_get1(ntru_sk_trits(n)),
       * 1));*/
      ntru->f = f;
      TEST_ASSERT_TRUE(!ntru_decr(ntru, spongos, ekey, dekey));
      TEST_ASSERT_TRUE(!trits_cmp_eq(key, dekey));
      ntru->f = f0;
      /*trits_put1(ntru_sk_trits(n), trit_sub(trits_get1(ntru_sk_trits(n)),
       * 1));*/

      trits_from_str(nonce, "NONCE9KEY9");
      prng_gen(prng, MAM2_PRNG_DST_SEC_KEY, nonce, key);
    } while (0 != (++i % (test_count / 10)));
  } while (++i < test_count);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(ntru_test);
  RUN_TEST(ntru_decr_fail_test);

  return UNITY_END();
}
