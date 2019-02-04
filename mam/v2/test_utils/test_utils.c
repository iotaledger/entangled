/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mam/v2/test_utils/test_utils.h"

mam_sponge_t *test_mam_sponge_init(test_mam_sponge_t *s) {
  memset(s->s.stack, 0, MAM2_SPONGE_WIDTH);
  memset(s->s.state, 0, MAM2_SPONGE_WIDTH);
  return &s->s;
}

mam_spongos_t *test_mam_spongos_init(test_mam_spongos_t *sg, mam_sponge_t *s) {
  return sg;
}

mam_wots_t *test_mam_wots_init(test_mam_wots_t *w) {
  memset(w->wots.secret_key, 0, MAM2_WOTS_SK_PART_SIZE);
  return &w->wots;
}

void prng_gen_str(mam_prng_t *p, trint3_t d, char const *nonce, trits_t Y) {
  size_t n;
  MAM2_TRITS_DEF0(N, MAM2_SPONGE_RATE);
  N = MAM2_TRITS_INIT(N, MAM2_SPONGE_RATE);

  n = strlen(nonce) * 3;
  N = trits_take_min(N, n);
  trits_from_str(N, nonce);

  mam_prng_gen(p, d, N, Y);
}

void _mam_sponge_hash(size_t Xn, char *X, size_t Yn, char *Y) {
  test_mam_sponge_t _s[1];
  mam_sponge_t *s = test_mam_sponge_init(_s);

  trits_t tX = trits_alloc(3 * Xn);
  trits_t tY = trits_alloc(3 * Yn);

  mam_sponge_init(s);
  trits_from_str(tX, X);
  mam_sponge_absorb(s, MAM2_SPONGE_CTL_DATA, tX);
  mam_sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, tY);
  trits_to_str(tY, Y);

  trits_free(tX);
  trits_free(tY);
}

bool _sponge_point_test() {
  bool r = true, ok;
  test_mam_sponge_t _s[1];
  mam_sponge_t *s = test_mam_sponge_init(_s);

  MAM2_TRITS_DEF0(K, MAM2_SPONGE_KEY_SIZE);
  MAM2_TRITS_DEF0(X, 2 * MAM2_SPONGE_RATE + 3);
  MAM2_TRITS_DEF0(Y, 2 * MAM2_SPONGE_RATE + 3);
  MAM2_TRITS_DEF0(T, 2 * MAM2_SPONGE_RATE + 3);
  trits_t x, y, t;

  char const rnd[(MAM2_SPONGE_KEY_SIZE + 2 * MAM2_SPONGE_RATE + 3) / 3 + 1] =
      "SERQWES9QWEFYSDKVJKPOKWFJKL"
      "JD9QW9EAWZMAJLQKJWEO9SDFPWA"
      "LKDIXNKISDFJ9ENKMLPQIOMLZQE"

      "SDFLJKWERQWESDIFYSDKVJKPOKL"
      "JZMAJLQKJWEO9SDFQPPQIOWEAMZ"
      "XNKISDF9QWELKJ99QW9EAWENKML"
      "QWPEOIERTYIASDLKJZXCMNSDF9N"
      "K9ASDPLZXCMSDFWEROIUCXVJKSF"
      "9REWRVXCVMAEKRUOIHEWLZXCPVS"

      "FLJSDKWEDIFYKVJKLKNWELKJ99Q"
      "JZMAJLQKRQWESSDFQPPQIOWEAMZ"
      "JZXCMNSXISDF9QW9EAWUOIHEEPO"
      "NKQWPETYIASDLKDFSSJWE9NKMLR"
      "RVXCVMAK9ASRLZXC9DDFWEOIUCX"
      "9REWEKRWLZXDPOIEMOCPVSVJKSF"

      "Z";
  char const hash_y[MAM2_SPONGE_HASH_SIZE / 3 + 1] =
      "99B9WYVWFGZVQTFAQ9NLWNNFYDJ"
      "AIGVWKROPLKSXS9TAPIGVUBHBZJ"
      "KRMLVUHKYZIIVQ9GTQSIUXXCSYS";
  char const encr_y[(2 * MAM2_SPONGE_RATE + 3) / 3 + 1] =
      "SDHFADDVJQPMXJGYZCNUXWZ9TQV"
      "UKTBJBPKOXMNMQKRJ9WPNLWXBSO"
      "BONTTMLNDEXGQQ9EGJMQNPNKOPK"
      "JMNEOIERTYIASDLKJZXCMNSDF9N"
      "K9ASDPLZXCMSDFWEROIUCXVJKSF"
      "9REWRVXCVMAEKRUOIHEWLZXCPVS"
      "ADEENUZFXEJRXGPZAFNXQFYRZWC"
      "WMEDUV9LADHFTXFPJTGZUNHRTG9"
      "HEFVXMWJWDWSUQUFP9HMRLC9BRN"
      "LTJDKVHGNSLZULTJDCQYKC9ABGX"
      "RIHCTBB9L9PUPRAIWIVCEZRFGMH"
      "SXENVJIRIBSENBTWYLLX9OYIHVD"
      "R";
  char const decr_y[(2 * MAM2_SPONGE_RATE + 3) / 3 + 1] =
      "SDMISU99ZQFIKPKNCEIDHUNKSWB"
      "WNCXJVUKBYIPNRUC9KFUD9WPXYM"
      "APZYOVIMFNPHW99VCUNBLFWZAAM"
      "9ROEOIERTYIASDLKJZXCMNSDF9N"
      "K9ASDPLZXCMSDFWEROIUCXVJKSF"
      "9REWRVXCVMAEKRUOIHEWLZXCPVS"
      "TTBHLYTXYRCKIXIBUZRIOSBOPFL"
      "9YTJFXJCGFAETMCXJCOVGUASFZB"
      "OITLBKGBSCKKYUPDXGJGYRKLZHZ"
      "MDQEA9WPDJWXCMLKGE9XHXMOUHL"
      "RWVCUUXPOZVOZJBODNMRNTULZTV"
      "HLEE9F9MFWKROJYNS9UEETJHEPQ"
      "M";

  K = MAM2_TRITS_INIT(K, MAM2_SPONGE_KEY_SIZE);
  X = MAM2_TRITS_INIT(X, 2 * MAM2_SPONGE_RATE + 3);
  Y = MAM2_TRITS_INIT(Y, 2 * MAM2_SPONGE_RATE + 3);
  T = MAM2_TRITS_INIT(T, 2 * MAM2_SPONGE_RATE + 3);

  ok = trits_from_str(K, rnd);
  MAM2_ASSERT(ok);
  ok = trits_from_str(X, rnd + MAM2_SPONGE_KEY_SIZE / 3);
  MAM2_ASSERT(ok);
  trits_set_zero(Y);

  mam_sponge_init(s);
  x = X;
  mam_sponge_absorb(s, MAM2_SPONGE_CTL_DATA, x);
  y = trits_take(Y, MAM2_SPONGE_HASH_SIZE);
  mam_sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, y);
  t = trits_take(T, MAM2_SPONGE_HASH_SIZE);
  ok = trits_from_str(t, hash_y);
  MAM2_ASSERT(ok);
  r = trits_cmp_eq(y, t) && r;

  mam_sponge_init(s);
  mam_sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  x = X;
  y = Y;
  mam_sponge_encr(s, x, y);
  ok = trits_from_str(t, encr_y);
  MAM2_ASSERT(ok);
  r = trits_cmp_eq(y, t) && r;

  mam_sponge_init(s);
  mam_sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  x = X;
  y = Y;
  trits_copy(x, y);
  mam_sponge_encr(s, y, y);
  ok = trits_from_str(t, encr_y);
  MAM2_ASSERT(ok);
  r = trits_cmp_eq(y, t) && r;

  mam_sponge_init(s);
  mam_sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  x = X;
  y = Y;
  mam_sponge_decr(s, x, y);
  ok = trits_from_str(t, decr_y);
  MAM2_ASSERT(ok);
  r = trits_cmp_eq(y, t) && r;

  mam_sponge_init(s);
  mam_sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  x = X;
  y = Y;
  trits_copy(x, y);
  mam_sponge_decr(s, y, y);
  ok = trits_from_str(t, decr_y);
  MAM2_ASSERT(ok);
  r = trits_cmp_eq(y, t) && r;

  return r;
}

void _mam_sponge_encr(size_t Kn, char *K, size_t Xn, char *X, size_t Yn,
                      char *Y) {
  test_mam_sponge_t _s[1];
  mam_sponge_t *s = test_mam_sponge_init(_s);

  trits_t tK = trits_alloc(3 * Kn);
  trits_t tX = trits_alloc(3 * Xn);
  trits_t tY = trits_alloc(3 * Yn);

  mam_sponge_init(s);
  trits_from_str(tK, K);
  mam_sponge_absorb(s, MAM2_SPONGE_CTL_KEY, tK);
  trits_from_str(tX, X);
  mam_sponge_encr(s, tX, tY);
  trits_to_str(tY, Y);

  trits_free(tK);
  trits_free(tX);
  trits_free(tY);
}

void _mam_sponge_decr(size_t Kn, char *K, size_t Yn, char *Y, size_t Xn,
                      char *X) {
  test_mam_sponge_t _s[1];
  mam_sponge_t *s = test_mam_sponge_init(_s);

  trits_t tK = trits_alloc(3 * Kn);
  trits_t tY = trits_alloc(3 * Yn);
  trits_t tX = trits_alloc(3 * Xn);

  mam_sponge_init(s);
  trits_from_str(tK, K);
  mam_sponge_absorb(s, MAM2_SPONGE_CTL_KEY, tK);
  trits_from_str(tY, Y);
  mam_sponge_decr(s, tY, tX);
  trits_to_str(tX, X);

  trits_free(tK);
  trits_free(tY);
  trits_free(tX);
}

void _prng_gen(size_t Kn, char *K, size_t Nn, char *N, size_t Yn, char *Y) {
  mam_prng_t p;

  trits_t tK = trits_alloc(3 * Kn);
  trits_t tN = trits_alloc(3 * Nn);
  trits_t tY = trits_alloc(3 * Yn);

  trits_from_str(tK, K);
  mam_prng_init(&p, tK);
  trits_from_str(tN, N);
  mam_prng_gen(&p, 0, tN, tY);
  trits_to_str(tY, Y);

  trits_free(tK);
  trits_free(tN);
  trits_free(tY);
}

void _mam_wots_gen_sign(size_t Kn, char *K, size_t Nn, char *N, size_t pkn,
                        char *pk, size_t Hn, char *H, size_t sign, char *sig) {
  test_mam_sponge_t _s[1];
  test_mam_wots_t _w[1];

  mam_sponge_t *s = test_mam_sponge_init(_s);
  mam_prng_t p;
  mam_wots_t *w = test_mam_wots_init(_w);

  trits_t tK = trits_alloc(3 * Kn);
  trits_t tN = trits_alloc(3 * Nn);
  trits_t tpk = trits_alloc(3 * pkn);
  trits_t tH = trits_alloc(3 * Hn);
  trits_t tsig = trits_alloc(3 * sign);

  trits_from_str(tK, K);
  mam_prng_init(&p, tK);
  trits_from_str(tN, N);
  mam_wots_gen_sk(w, &p, tN);
  mam_wots_calc_pk(w, tpk);
  trits_to_str(tpk, pk);

  trits_from_str(tH, H);
  mam_wots_sign(w, tH, tsig);
  trits_to_str(tsig, sig);

  trits_free(tK);
  trits_free(tN);
  trits_free(tpk);
  trits_free(tH);
  trits_free(tsig);
}

void test_gen_sponge(mam_prng_t *p, mam_sponge_t *s) {
#define TEST_MAX_SIZE (MAM2_SPONGE_RATE * 3)
  MAM2_TRITS_DEF0(K, MAM2_SPONGE_KEY_SIZE);
  MAM2_TRITS_DEF0(X, TEST_MAX_SIZE);
  MAM2_TRITS_DEF0(Y, TEST_MAX_SIZE);
  MAM2_TRITS_DEF0(H, MAM2_SPONGE_RATE);
  trits_t x, y;
  size_t i, ns[] = {0, 243, 487, TEST_MAX_SIZE + 1};

  K = MAM2_TRITS_INIT(K, MAM2_SPONGE_KEY_SIZE);
  X = MAM2_TRITS_INIT(X, TEST_MAX_SIZE);
  Y = MAM2_TRITS_INIT(Y, TEST_MAX_SIZE);
  H = MAM2_TRITS_INIT(H, MAM2_SPONGE_RATE);

  prng_gen_str(p, MAM2_PRNG_DST_SEC_KEY, "SPONGEKEY", K);
  prng_gen_str(p, 4, "SPONGEDATA", X);

  printf("sponge\n");
  for (i = 0; ns[i] < TEST_MAX_SIZE; ++i) {
    x = trits_take(X, ns[i]);
    y = trits_take(Y, ns[i]);

    mam_sponge_init(s);
    mam_sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
    mam_sponge_encr(s, x, y);
    mam_sponge_squeeze(s, MAM2_SPONGE_CTL_MAC, H);

    trits_print2("\tK\t=", K, "\n");
    trits_print2("\tx\t=", x, "\n");
    trits_print2("\ty\t=", y, "\n");
    trits_print2("\tH\t=", H, "\n");
    printf("----------\n");
  }
  printf("\n");
#undef TEST_MAX_SIZE
}

void test_gen_spongos(mam_prng_t *p, mam_spongos_t *s) {
#define TEST_MAX_SIZE (MAM2_SPONGE_RATE * 3)
  MAM2_TRITS_DEF0(K, MAM2_SPONGE_KEY_SIZE);
  MAM2_TRITS_DEF0(X, TEST_MAX_SIZE);
  MAM2_TRITS_DEF0(Y, TEST_MAX_SIZE);
  MAM2_TRITS_DEF0(H, MAM2_SPONGE_RATE);
  trits_t x, y;
  size_t i, ns[] = {0, 243, 487, TEST_MAX_SIZE + 1};

  K = MAM2_TRITS_INIT(K, MAM2_SPONGE_KEY_SIZE);
  X = MAM2_TRITS_INIT(X, TEST_MAX_SIZE);
  Y = MAM2_TRITS_INIT(Y, TEST_MAX_SIZE);
  H = MAM2_TRITS_INIT(H, MAM2_SPONGE_RATE);

  prng_gen_str(p, MAM2_PRNG_DST_SEC_KEY, "SPONGEKEY", K);
  prng_gen_str(p, 4, "SPONGEDATA", X);

  printf("spongos\n");
  for (i = 0; ns[i] < TEST_MAX_SIZE; ++i) {
    x = trits_take(X, ns[i]);
    y = trits_take(Y, ns[i]);

    mam_spongos_init(s);
    mam_spongos_absorb(s, K);
    mam_spongos_commit(s);
    mam_spongos_encr(s, x, y);
    mam_spongos_commit(s);
    mam_spongos_squeeze(s, H);

    trits_print2("\tK\t=", K, "\n");
    trits_print2("\tx\t=", x, "\n");
    trits_print2("\ty\t=", y, "\n");
    trits_print2("\tH\t=", H, "\n");
    printf("----------\n");
  }
  printf("\n");
#undef TEST_MAX_SIZE
}

void test_gen_prng(mam_prng_t *p, mam_prng_t *r, mam_sponge_t *s) {
#define TEST_MAX_SIZE (MAM2_SPONGE_RATE * 3)
  MAM2_TRITS_DEF0(K, MAM2_SPONGE_KEY_SIZE);
  MAM2_TRITS_DEF0(N, TEST_MAX_SIZE);
  MAM2_TRITS_DEF0(X, TEST_MAX_SIZE);
  trits_t n, x;
  size_t i, ns[] = {0, 243, 487, TEST_MAX_SIZE + 1};

  K = MAM2_TRITS_INIT(K, MAM2_SPONGE_KEY_SIZE);
  N = MAM2_TRITS_INIT(N, TEST_MAX_SIZE);
  X = MAM2_TRITS_INIT(X, TEST_MAX_SIZE);

  prng_gen_str(p, MAM2_PRNG_DST_SEC_KEY, "PRNGKEY", K);
  prng_gen_str(p, 4, "PRNGNONCE", N);
  mam_prng_init(r, K);

  printf("prng\n");
  for (i = 0; ns[i] < TEST_MAX_SIZE; ++i) {
    n = trits_take_min(N, 33);
    x = trits_take(X, ns[i]);

    mam_prng_gen(r, MAM2_PRNG_DST_SEC_KEY, n, x);

    trits_print2("\tK\t=", K, "\n");
    trits_print2("\tn\t=", n, "\n");
    trits_print2("\tx\t=", x, "\n");
    printf("----------\n");
  }
  for (i = 0; ns[i] < TEST_MAX_SIZE; ++i) {
    n = trits_take(N, ns[i]);
    x = trits_take_min(X, 33);

    mam_prng_gen(r, MAM2_PRNG_DST_SEC_KEY, n, x);

    trits_print2("\tK\t=", K, "\n");
    trits_print2("\tn\t=", n, "\n");
    trits_print2("\tx\t=", x, "\n");
    printf("----------\n");
  }
  printf("\n");
#undef TEST_MAX_SIZE
}

void test_gen_wots(mam_prng_t *p, mam_wots_t *w, mam_sponge_t *s,
                   mam_prng_t *r) {
#define TEST_MAX_SIZE (MAM2_SPONGE_RATE * 3)
  MAM2_TRITS_DEF0(K, MAM2_SPONGE_KEY_SIZE);
  MAM2_TRITS_DEF0(N, TEST_MAX_SIZE);
  MAM2_TRITS_DEF0(Q, MAM2_WOTS_PK_SIZE);
  MAM2_TRITS_DEF0(H, MAM2_WOTS_HASH_SIZE);
  MAM2_TRITS_DEF0(X, MAM2_WOTS_SIG_SIZE);
  trits_t n;
  size_t i, ns[] = {0, 243, 487, TEST_MAX_SIZE + 1};

  K = MAM2_TRITS_INIT(K, MAM2_SPONGE_KEY_SIZE);
  N = MAM2_TRITS_INIT(N, TEST_MAX_SIZE);
  Q = MAM2_TRITS_INIT(Q, MAM2_WOTS_PK_SIZE);
  H = MAM2_TRITS_INIT(H, MAM2_WOTS_HASH_SIZE);
  X = MAM2_TRITS_INIT(X, MAM2_WOTS_SIG_SIZE);

  prng_gen_str(p, MAM2_PRNG_DST_SEC_KEY, "WOTSPRNGKEY", K);
  prng_gen_str(p, 4, "WOTSNONCE", N);

  mam_prng_init(r, K);

  printf("wots\n");
  trits_print2("\tK\t=", K, "\n");
  printf("----------\n");

  for (i = 0; ns[i] < TEST_MAX_SIZE; ++i) {
    n = trits_take(N, ns[i]);

    mam_wots_gen_sk(w, r, n);
    mam_wots_calc_pk(w, Q);
    trits_print2("\tn\t=", n, "\n");
    trits_print2("\tpk\t=", Q, "\n");
    printf("----------\n");

    prng_gen_str(p, 4, "WOTSHASH", H);
    mam_wots_sign(w, H, X);
    trits_print2("\tH\t=", H, "\n");
    trits_print2("\tsig\t=", X, "\n");
    printf("----------\n");

    prng_gen_str(p, 4, "WOTSHASH9", H);
    mam_wots_sign(w, H, X);
    trits_print2("\tH\t=", H, "\n");
    trits_print2("\tsig\t=", X, "\n");
    printf("----------\n");
  }
  printf("\n");
#undef TEST_MAX_SIZE
}

#if 0
void test_gen_mss(prng_t *p, mss_t *m, prng_t *r, mam_sponge_t *spongos, mam_wots_t *w)
{
#define TEST_MAX_SIZE (MAM2_SPONGE_RATE * 3)
  MAM2_TRITS_DEF0(K, MAM2_SPONGE_KEY_SIZE);
  MAM2_TRITS_DEF0(N, TEST_MAX_SIZE);
  MAM2_TRITS_DEF0(Q, MAM2_WOTS_PK_SIZE);
  MAM2_TRITS_DEF0(H, MAM2_WOTS_HASH_SIZE);
  MAM2_TRITS_DEF0(X, MAM2_WOTS_SIG_SIZE);
  trits_t n;
  size_t i, spongos_ntru[] = { 0, 243, 487, TEST_MAX_SIZE+1 };

  K = MAM2_TRITS_INIT(K, MAM2_SPONGE_KEY_SIZE);
  N = MAM2_TRITS_INIT(N, TEST_MAX_SIZE);
  Q = MAM2_TRITS_INIT(Q, MAM2_WOTS_PK_SIZE);
  H = MAM2_TRITS_INIT(H, MAM2_WOTS_HASH_SIZE);
  X = MAM2_TRITS_INIT(X, MAM2_WOTS_SIG_SIZE);

  prng_gen_str(p, MAM2_PRNG_DST_SEC_KEY, "WOTSPRNGKEY", K);
  prng_gen_str(p, 4, "WOTSNONCE", N);

  printf("mss\n");
  trits_print2("\tK\t=", K, "\n");
  printf("----------\n");

  for(i = 0; spongos_ntru[i] < TEST_MAX_SIZE; ++i)
  {
    mss_init(m, r, spongos, w, d, n1, n2);
    mss_gen(m, pk);
    mss_sign(m, H, sig);
    mss_save(m, );

    n = trits_take(N, spongos_ntru[i]);

    mam_wots_gen_sk(w, r, n);
    mam_wots_calc_pk(w, Q);
    trits_print2("\tn\t=", n, "\n");
    trits_print2("\tpk\t=", Q, "\n");
    printf("----------\n");

    prng_gen_str(p, 4, "WOTSHASH", H);
    mam_wots_sign(w, H, X);
    trits_print2("\tH\t=", H, "\n");
    trits_print2("\tsig\t=", X, "\n");
    printf("----------\n");

    prng_gen_str(p, 4, "WOTSHASH9", H);
    mam_wots_sign(w, H, X);
    trits_print2("\tH\t=", H, "\n");
    trits_print2("\tsig\t=", X, "\n");
    printf("----------\n");
  }
  printf("\n");
#undef TEST_MAX_SIZE
}

void test_gen_ntru(prng_t *p, ntru_t *n)
{
#define TEST_MAX_SIZE (MAM2_SPONGE_RATE * 3)
  MAM2_TRITS_DEF0(K, MAM2_PRNG_KEY_SIZE);
  MAM2_TRITS_DEF0(SK, MAM2_NTRU_SK_SIZE);
  MAM2_TRITS_DEF0(PK, MAM2_NTRU_PK_SIZE);
  MAM2_TRITS_DEF0(X, MAM2_NTRU_KEY_SIZE);
  MAM2_TRITS_DEF0(Y, MAM2_NTRU_EKEY_SIZE);
  MAM2_TRITS_DEF0(N, TEST_MAX_SIZE);
  trits_t x, y;
  size_t i, spongos_ntru[] = { 0, 243, 487, TEST_MAX_SIZE+1 };

  K = MAM2_TRITS_INIT(K, MAM2_PRNG_KEY_SIZE);
  SK = MAM2_TRITS_INIT(SK, MAM2_NTRU_SK_SIZE);
  PK = MAM2_TRITS_INIT(PK, MAM2_NTRU_PK_SIZE);
  X = MAM2_TRITS_INIT(X, MAM2_NTRU_KEY_SIZE);
  Y = MAM2_TRITS_INIT(Y, MAM2_NTRU_EKEY_SIZE);
  N = MAM2_TRITS_INIT(N, TEST_MAX_SIZE);

  prng_gen_str(p, MAM2_PRNG_DST_SEC_KEY, "NTRUGENNONCE", n);
  prng_gen_str(p, MAM2_PRNG_DST_SEC_KEY, "NTRUENCRNONCE", n);
  prng_gen_str(p, MAM2_PRNG_DST_SEC_KEY, "NTRUPRNGKEY", K);
  prng_gen_str(p, MAM2_PRNG_DST_SEC_KEY, "NTRUKEY", X);

  prng_init(r, K);

  printf("ntru\n");
  trits_print2("\trngK\t=", K, "\n");
  printf("----------\n");

  for(i = 0; spongos_ntru[i] < TEST_MAX_SIZE; ++i)
  {
    n = trits_take(N, spongos_ntru[i]);

    ntru_gen(n, r, n, PK);
    trits_print2("\tn\t=", n, "\n");
    trits_print2("\tsk\t=", ntru_sk_trits(n), "\n");
    trits_print2("\tpk\t=", PK, "\n");
    printf("----------\n");

    ntru_encr(Q, r, spongos, X, N, Y);
    trits_print2("\tX\t=", X, "\n");
    trits_print2("\tn\t=", n, "\n");
    trits_print2("\tY\t=", Y, "\n");
    printf("----------\n");
  }
  printf("\n");
#undef TEST_MAX_SIZE
}
#endif

// bool _gen() {
//   test_mam_sponge_t _s[1];
//   test_mam_spongos_t _sg[1];
//   test_prng_t _p[1], _pa[1], _pb[1];
//   test_mam_wots_t _w[1];
//   test_mss1 _m1[1];
//   test_mss2 _m2[1];
//   test_mss3 _m3[1];
//   test_mss4 _m4[1];
//   test_mss _m[1];
//   test_ntru_t _n[1];
//
//   mam_sponge_t *spongos = test_mam_sponge_init(_s);
//   mam_spongos_t *sg = test_mam_spongos_init(_sg, spongos);
//   prng_t *p = test_prng_init(_p, spongos);
//   prng_t *pa = test_prng_init(_pa, spongos);
//   prng_t *pb = test_prng_init(_pb, spongos);
//   mam_wots_t *w = test_mam_wots_init(_w, spongos);
//   mss_t *m1 = test_mss_init1(_m1);
//   mss_t *m2 = test_mss_init2(_m2);
//   mss_t *m3 = test_mss_init3(_m3);
//   mss_t *m4 = test_mss_init4(_m4);
//   mss_t *m = test_mss_init(_m);
//   ntru_t *n = test_ntru_init(_n);
//
// #if 0
//   test_gen_sponge(p, spongos);
//   test_gen_spongos(p, spongos);
//   test_gen_prng(p, r, spongos);
//   test_gen_wots(p, w, spongos, r);
//   test_gen_mss(p, m, r, spongos, w);
//   test_gen_ntru(p, n);
// #endif
//
//   return 1;
// }
//
// bool _() {
//   test_sponge _s[1];
//   test_spongos _sg[1];
//   test_prng _p[1], _pa[1], _pb[1];
//   test_wots _w[1];
//   test_mss1 _m1[1];
//   test_mss2 _m2[1];
//   test_mss3 _m3[1];
//   test_mss4 _m4[1];
//   test_mss4 _m42[1];
//   test_mss5 _m5[1];
//   test_mssx _mx[1];
//   test_mss _m[1];
//   test_ntru _n[1];
//
//   mam_sponge_t *spongos = test_mam_sponge_init(_s);
//   mam_spongos_t *sg = test_mam_spongos_init(_sg, spongos);
//   prng_t *p = test_prng_init(_p, spongos);
//   prng_t *pa = test_prng_init(_pa, spongos);
//   prng_t *pb = test_prng_init(_pb, spongos);
//   mam_wots_t *w = test_mam_wots_init(_w, spongos);
//   mss_t *m1 = test_mss_init1(_m1);
//   mss_t *m2 = test_mss_init2(_m2);
//   mss_t *m3 = test_mss_init3(_m3);
//   mss_t *m4 = test_mss_init4(_m4);
//   mss_t *m42 = test_mss_init4(_m42);
//   mss_t *m5 = test_mss_init5(_m5);
//   mss_t *mx = test_mss_initx(_mx);
//   mss_t *m = test_mss_init(_m);
//   ntru_t *n = test_ntru_init(_n);
//
//   bool r = true, rr;
//   clock_t clk;
// #define __run_test(n, f)                                \
//   do {                                                  \
//     printf(n);                                          \
//     rr = f;                                             \
//     r = r && rr;                                        \
//   } while (0)
//
//   __run_test("MAM2   ",
//              mam2_test(spongos, 0, test_create_sponge, test_delete_sponge,
//              pa, pb));
//   return r;
// }

// void test() {
//   char N[12 + 1] = "9ABCKNLMOXYZ";
//
//   char K[81 + 1] =
//       "ABCKNLMOXYZ99AZNODFGWERXCVH"
//       "XCVHABCKNLMOXYZ99AZNODFGWER"
//       "FGWERXCVHABCKNLMOXYZ99AZNOD";
//
//   char H[78 + 1] =
//       "BACKNLMOXYZ99AZNODFGWERXCVH"
//       "XCVHABCKNLMOXYZ99AZNODFGWER"
//       "FGWERXCVHABCKNLMOXYZ99AZ";
//
//   char X[162 + 1] =
//       "ABCKNLMOXYZ99AZNODFGWERXCVH"
//       "XCVHABCKNLMOXYZ99AZNODFGWER"
//       "FGWERXCVHABCKNLMOXYZ99AZNOD"
//       "Z99AZNODFGABCKNLMOXYWERXCVH"
//       "AZNODFGXCVKNLMOXHABCYZ99WER"
//       "FGWERVHABCKXCNLM9AZNODOXYZ9";
//
//   bool r = true;
//   size_t mam_sponge_hash_Yn = 81;
//   size_t mam_sponge_encr_Yn = 162;
//   size_t mam_sponge_decr_Xn = 162;
//   size_t prng_gen_Yn = 162;
//
//   char mam_sponge_hash_Y[81];
//   char mam_sponge_encr_Y[162];
//   char mam_sponge_decr_X[162];
//   char prng_gen_Y[162];
//   char mam_wots_gen_sign_pk[MAM2_WOTS_PK_SIZE / 3];
//   char mam_wots_gen_sign_sig[MAM2_WOTS_SIG_SIZE / 3];
//
//   do {
//     r = _() && r;
//     r = sponge_point_test() && r;
//     mam_sponge_hash(162, X, mam_sponge_hash_Yn, mam_sponge_hash_Y);
//     mam_sponge_encr(81, K, 162, X, mam_sponge_encr_Yn, mam_sponge_encr_Y);
//     mam_sponge_decr(81, K, 162, X, mam_sponge_decr_Xn, mam_sponge_decr_X);
//     prng_gen(81, K, 12, N, prng_gen_Yn, prng_gen_Y);
//     mam_wots_gen_sign(81, K, 12, N, MAM2_WOTS_PK_SIZE / 3,
//     mam_wots_gen_sign_pk, 78, H,
//                   MAM2_WOTS_SIG_SIZE / 3, mam_wots_gen_sign_sig);
//   } while (0)
// }
//
// int main() {
//   test();
//   return 0;
// }
