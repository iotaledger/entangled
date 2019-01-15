/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <time.h>

#include "mam/v2/alloc.h"
#include "mam/v2/test_utils/test_utils.h"

void test_f(void *buf, word_t *s) {
  trits_t x = trits_from_rep(MAM2_SPONGE_RATE, s);
  trits_t y = trits_from_rep(MAM2_SPONGE_RATE, (word_t *)buf);
  trits_t x0 = trits_take(x, MAM2_SPONGE_RATE / 2);
  trits_t x1 = trits_drop(x, MAM2_SPONGE_RATE / 2);
  trits_t x2 =
      trits_drop(trits_from_rep(MAM2_SPONGE_WIDTH, s), MAM2_SPONGE_RATE);

  trits_add(x0, x1, x0);
  trits_add(x0, x2, x0);
  trits_add(x0, x2, x2);

  trits_copy(trits_take(x, MAM2_SPONGE_RATE - 6), trits_drop(y, 6));
  trits_copy(trits_drop(x, MAM2_SPONGE_RATE - 6), trits_take(y, 6));
  trits_copy(y, x);
}

intru *test_ntru_init(test_ntru_t *n) {
  n->n.id = n->id;
  n->n.sk = n->sk;
  n->n.f = n->f;
  return &n->n;
}

iprng *test_prng_init(test_prng_t *p, isponge *s) {
  p->p.s = s;
  p->p.k = p->key;
  return &p->p;
}

isponge *test_sponge_init(test_sponge_t *s) {
  s->s.f = test_f;
  s->s.stack = s->stack;
  s->s.s = s->state;
  return &s->s;
}

ispongos *test_spongos_init(test_spongos_t *sg, isponge *s) {
  sg->s = s;
  return sg;
}

iwots *test_wots_init(test_wots_t *w, isponge *s) {
  w->w.sg->s = s;
  w->w.sk = w->sk;
  return &w->w;
}

#if !defined(MAM2_MSS_TEST_MAX_D)
#define MAM2_MSS_TEST_MAX_D 3
#endif

#if defined(MAM2_MSS_TRAVERSAL)
#define def_test_mss(D, sfx)                  \
  typedef struct _test_mss##sfx {             \
    imss m;                                   \
    word_t ap[MAM2_MSS_MT_AUTH_WORDS(D)];     \
    uint32_t ap_check;                        \
    word_t hs[MAM2_MSS_MT_HASH_WORDS(D, 1)];  \
    uint32_t hs_check;                        \
    mss_mt_node ns[MAM2_MSS_MT_NODES(D) + 1]; \
    uint32_t ns_check;                        \
    mss_mt_stack ss[MAM2_MSS_MT_STACKS(D)];   \
    uint32_t ss_check;                        \
  } test_mss##sfx
#else
#define def_test_mss(D, sfx)         \
  typedef struct _test_mss##sfx {    \
    imss m;                          \
    word_t mt[MAM2_MSS_MT_WORDS(D)]; \
    uint32_t mt_check;               \
  } test_mss##sfx
#endif

#if defined(MAM2_MSS_TRAVERSAL)
#define def_test_mss_init(D, sfx)                     \
  static imss *test_mss_init##sfx(test_mss##sfx *m) { \
    m->m.ap = m->ap;                                  \
    m->ap_check = 0xdeadbeef;                         \
    m->m.hs = m->hs;                                  \
    m->hs_check = 0xdeadbeef;                         \
    m->m.ns = m->ns;                                  \
    m->ns_check = 0xdeadbeef;                         \
    m->m.ss = m->ss;                                  \
    m->ss_check = 0xdeadbeef;                         \
    return &m->m;                                     \
  }
#else
#define def_test_mss_init(D, sfx)                               \
  static imss *test_mss_init##sfx(test_mss##sfx *m, iwots *w) { \
    m->m.d = D;                                                 \
    m->m.ws = w;                                                \
    m->m.mt = m->mt;                                            \
    m->mt_check = 0xdeadbeef;                                   \
    return &m->m;                                               \
  }
#endif

#if defined(MAM2_MSS_TRAVERSAL)
#define def_test_mss_check(D, sfx)                                        \
  static bool_t test_mss_check##sfx(test_mss##sfx *m) {                   \
    return 1 && m->ap_check == 0xdeadbeef && m->hs_check == 0xdeadbeef && \
           m->ns_check == 0xdeadbeef && m->ss_check == 0xdeadbeef;        \
  }
#else
#define def_test_mss_check(D, sfx)                      \
  static bool_t test_mss_check##sfx(test_mss##sfx *m) { \
    return 1 && m->mt_check == 0xdeadbeef;              \
  }
#endif

def_test_mss(1, 1);
def_test_mss(2, 2);
def_test_mss(3, 3);
def_test_mss(4, 4);
def_test_mss(5, 5);
def_test_mss(10, x);
def_test_mss(MAM2_MSS_TEST_MAX_D, );

def_test_mss_init(1, 1);
def_test_mss_init(2, 2);
def_test_mss_init(3, 3);
def_test_mss_init(4, 4);
def_test_mss_init(5, 5);
def_test_mss_init(10, x);
def_test_mss_init(MAM2_MSS_TEST_MAX_D, );

def_test_mss_check(1, 1);
def_test_mss_check(2, 2);
def_test_mss_check(3, 3);
def_test_mss_check(4, 4);
def_test_mss_check(5, 5);
def_test_mss_check(10, x);
def_test_mss_check(MAM2_MSS_TEST_MAX_D, );

static char *bool_str(bool_t b) {
  static char *yes = "ok";
  static char *no = "failed";
  return b ? yes : no;
}

void _sponge_hash(size_t Xn, char *X, size_t Yn, char *Y) {
  test_sponge_t _s[1];
  isponge *s = test_sponge_init(_s);

  ialloc *a = 0;
  trits_t tX = trits_alloc(a, 3 * Xn);
  trits_t tY = trits_alloc(a, 3 * Yn);

  sponge_init(s);
  trits_from_str(tX, X);
  sponge_absorb(s, MAM2_SPONGE_CTL_DATA, tX);
  sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, tY);
  trits_to_str(tY, Y);

  trits_free(a, tX);
  trits_free(a, tY);
}

bool_t _sponge_point_test() {
  bool_t r = 1, ok;
  test_sponge_t _s[1];
  isponge *s = test_sponge_init(_s);

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

  sponge_init(s);
  x = X;
  sponge_absorb(s, MAM2_SPONGE_CTL_DATA, x);
  y = trits_take(Y, MAM2_SPONGE_HASH_SIZE);
  sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, y);
  t = trits_take(T, MAM2_SPONGE_HASH_SIZE);
  ok = trits_from_str(t, hash_y);
  MAM2_ASSERT(ok);
  r = trits_cmp_eq(y, t) && r;

  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  x = X;
  y = Y;
  sponge_encr(s, x, y);
  ok = trits_from_str(t, encr_y);
  MAM2_ASSERT(ok);
  r = trits_cmp_eq(y, t) && r;

  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  x = X;
  y = Y;
  trits_copy(x, y);
  sponge_encr(s, y, y);
  ok = trits_from_str(t, encr_y);
  MAM2_ASSERT(ok);
  r = trits_cmp_eq(y, t) && r;

  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  x = X;
  y = Y;
  sponge_decr(s, x, y);
  ok = trits_from_str(t, decr_y);
  MAM2_ASSERT(ok);
  r = trits_cmp_eq(y, t) && r;

  sponge_init(s);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
  x = X;
  y = Y;
  trits_copy(x, y);
  sponge_decr(s, y, y);
  ok = trits_from_str(t, decr_y);
  MAM2_ASSERT(ok);
  r = trits_cmp_eq(y, t) && r;

  return r;
}

void _sponge_encr(size_t Kn, char *K, size_t Xn, char *X, size_t Yn, char *Y) {
  test_sponge_t _s[1];
  isponge *s = test_sponge_init(_s);

  ialloc *a = 0;
  trits_t tK = trits_alloc(a, 3 * Kn);
  trits_t tX = trits_alloc(a, 3 * Xn);
  trits_t tY = trits_alloc(a, 3 * Yn);

  sponge_init(s);
  trits_from_str(tK, K);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, tK);
  trits_from_str(tX, X);
  sponge_encr(s, tX, tY);
  trits_to_str(tY, Y);

  trits_free(a, tK);
  trits_free(a, tX);
  trits_free(a, tY);
}

void _sponge_decr(size_t Kn, char *K, size_t Yn, char *Y, size_t Xn, char *X) {
  test_sponge_t _s[1];
  isponge *s = test_sponge_init(_s);

  ialloc *a = 0;
  trits_t tK = trits_alloc(a, 3 * Kn);
  trits_t tY = trits_alloc(a, 3 * Yn);
  trits_t tX = trits_alloc(a, 3 * Xn);

  sponge_init(s);
  trits_from_str(tK, K);
  sponge_absorb(s, MAM2_SPONGE_CTL_KEY, tK);
  trits_from_str(tY, Y);
  sponge_decr(s, tY, tX);
  trits_to_str(tX, X);

  trits_free(a, tK);
  trits_free(a, tY);
  trits_free(a, tX);
}

void _prng_gen(size_t Kn, char *K, size_t Nn, char *N, size_t Yn, char *Y) {
  test_sponge_t _s[1];
  test_prng_t _p[1];
  isponge *s = test_sponge_init(_s);
  iprng *p = test_prng_init(_p, s);

  ialloc *a = 0;
  trits_t tK = trits_alloc(a, 3 * Kn);
  trits_t tN = trits_alloc(a, 3 * Nn);
  trits_t tY = trits_alloc(a, 3 * Yn);

  trits_from_str(tK, K);
  prng_init(p, s, tK);
  trits_from_str(tN, N);
  prng_gen(p, 0, tN, tY);
  trits_to_str(tY, Y);

  trits_free(a, tK);
  trits_free(a, tN);
  trits_free(a, tY);
}

void _wots_gen_sign(size_t Kn, char *K, size_t Nn, char *N, size_t pkn,
                    char *pk, size_t Hn, char *H, size_t sign, char *sig) {
  test_sponge_t _s[1];
  test_spongos_t _sg[1];
  test_prng_t _p[1];
  test_wots_t _w[1];
  isponge *s = test_sponge_init(_s);
  ispongos *sg = test_spongos_init(_sg, s);
  iprng *p = test_prng_init(_p, s);
  iwots *w = test_wots_init(_w, s);

  ialloc *a = 0;
  trits_t tK = trits_alloc(a, 3 * Kn);
  trits_t tN = trits_alloc(a, 3 * Nn);
  trits_t tpk = trits_alloc(a, 3 * pkn);
  trits_t tH = trits_alloc(a, 3 * Hn);
  trits_t tsig = trits_alloc(a, 3 * sign);

  trits_from_str(tK, K);
  prng_init(p, s, tK);
  trits_from_str(tN, N);
  wots_gen_sk(w, p, tN);
  wots_calc_pk(w, tpk);
  trits_to_str(tpk, pk);

  trits_from_str(tH, H);
  wots_sign(w, tH, tsig);
  trits_to_str(tsig, sig);

  trits_free(a, tK);
  trits_free(a, tN);
  trits_free(a, tpk);
  trits_free(a, tH);
  trits_free(a, tsig);
}

void test_gen_sponge(iprng *p, isponge *s) {
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

  prng_gen_str(p, MAM2_PRNG_DST_SECKEY, "SPONGEKEY", K);
  prng_gen_str(p, 4, "SPONGEDATA", X);

  printf("sponge\n");
  for (i = 0; ns[i] < TEST_MAX_SIZE; ++i) {
    x = trits_take(X, ns[i]);
    y = trits_take(Y, ns[i]);

    sponge_init(s);
    sponge_absorb(s, MAM2_SPONGE_CTL_KEY, K);
    sponge_encr(s, x, y);
    sponge_squeeze(s, MAM2_SPONGE_CTL_MAC, H);

    trits_print2("\tK\t=", K, "\n");
    trits_print2("\tx\t=", x, "\n");
    trits_print2("\ty\t=", y, "\n");
    trits_print2("\tH\t=", H, "\n");
    printf("----------\n");
  }
  printf("\n");
#undef TEST_MAX_SIZE
}

void test_gen_spongos(iprng *p, ispongos *s) {
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

  prng_gen_str(p, MAM2_PRNG_DST_SECKEY, "SPONGEKEY", K);
  prng_gen_str(p, 4, "SPONGEDATA", X);

  printf("spongos\n");
  for (i = 0; ns[i] < TEST_MAX_SIZE; ++i) {
    x = trits_take(X, ns[i]);
    y = trits_take(Y, ns[i]);

    spongos_init(s);
    spongos_absorb(s, K);
    spongos_commit(s);
    spongos_encr(s, x, y);
    spongos_commit(s);
    spongos_squeeze(s, H);

    trits_print2("\tK\t=", K, "\n");
    trits_print2("\tx\t=", x, "\n");
    trits_print2("\ty\t=", y, "\n");
    trits_print2("\tH\t=", H, "\n");
    printf("----------\n");
  }
  printf("\n");
#undef TEST_MAX_SIZE
}

void test_gen_prng(iprng *p, iprng *r, isponge *s) {
#define TEST_MAX_SIZE (MAM2_SPONGE_RATE * 3)
  MAM2_TRITS_DEF0(K, MAM2_SPONGE_KEY_SIZE);
  MAM2_TRITS_DEF0(N, TEST_MAX_SIZE);
  MAM2_TRITS_DEF0(X, TEST_MAX_SIZE);
  trits_t n, x;
  size_t i, ns[] = {0, 243, 487, TEST_MAX_SIZE + 1};

  K = MAM2_TRITS_INIT(K, MAM2_SPONGE_KEY_SIZE);
  N = MAM2_TRITS_INIT(N, TEST_MAX_SIZE);
  X = MAM2_TRITS_INIT(X, TEST_MAX_SIZE);

  prng_gen_str(p, MAM2_PRNG_DST_SECKEY, "PRNGKEY", K);
  prng_gen_str(p, 4, "PRNGNONCE", N);
  prng_init(r, s, K);

  printf("prng\n");
  for (i = 0; ns[i] < TEST_MAX_SIZE; ++i) {
    n = trits_take_min(N, 33);
    x = trits_take(X, ns[i]);

    prng_gen(r, MAM2_PRNG_DST_SECKEY, n, x);

    trits_print2("\tK\t=", K, "\n");
    trits_print2("\tn\t=", n, "\n");
    trits_print2("\tx\t=", x, "\n");
    printf("----------\n");
  }
  for (i = 0; ns[i] < TEST_MAX_SIZE; ++i) {
    n = trits_take(N, ns[i]);
    x = trits_take_min(X, 33);

    prng_gen(r, MAM2_PRNG_DST_SECKEY, n, x);

    trits_print2("\tK\t=", K, "\n");
    trits_print2("\tn\t=", n, "\n");
    trits_print2("\tx\t=", x, "\n");
    printf("----------\n");
  }
  printf("\n");
#undef TEST_MAX_SIZE
}

void test_gen_wots(iprng *p, iwots *w, isponge *s, iprng *r) {
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

  prng_gen_str(p, MAM2_PRNG_DST_SECKEY, "WOTSPRNGKEY", K);
  prng_gen_str(p, 4, "WOTSNONCE", N);

  wots_init(w, s);
  prng_init(r, s, K);

  printf("wots\n");
  trits_print2("\tK\t=", K, "\n");
  printf("----------\n");

  for (i = 0; ns[i] < TEST_MAX_SIZE; ++i) {
    n = trits_take(N, ns[i]);

    wots_gen_sk(w, r, n);
    wots_calc_pk(w, Q);
    trits_print2("\tn\t=", n, "\n");
    trits_print2("\tpk\t=", Q, "\n");
    printf("----------\n");

    prng_gen_str(p, 4, "WOTSHASH", H);
    wots_sign(w, H, X);
    trits_print2("\tH\t=", H, "\n");
    trits_print2("\tsig\t=", X, "\n");
    printf("----------\n");

    prng_gen_str(p, 4, "WOTSHASH9", H);
    wots_sign(w, H, X);
    trits_print2("\tH\t=", H, "\n");
    trits_print2("\tsig\t=", X, "\n");
    printf("----------\n");
  }
  printf("\n");
#undef TEST_MAX_SIZE
}

#if 0
void test_gen_mss(iprng *p, imss *m, iprng *r, isponge *s, iwots *w)
{
#define TEST_MAX_SIZE (MAM2_SPONGE_RATE * 3)
  MAM2_TRITS_DEF0(K, MAM2_SPONGE_KEY_SIZE);
  MAM2_TRITS_DEF0(N, TEST_MAX_SIZE);
  MAM2_TRITS_DEF0(Q, MAM2_WOTS_PK_SIZE);
  MAM2_TRITS_DEF0(H, MAM2_WOTS_HASH_SIZE);
  MAM2_TRITS_DEF0(X, MAM2_WOTS_SIG_SIZE);
  trits_t n;
  size_t i, ns[] = { 0, 243, 487, TEST_MAX_SIZE+1 };

  K = MAM2_TRITS_INIT(K, MAM2_SPONGE_KEY_SIZE);
  N = MAM2_TRITS_INIT(N, TEST_MAX_SIZE);
  Q = MAM2_TRITS_INIT(Q, MAM2_WOTS_PK_SIZE);
  H = MAM2_TRITS_INIT(H, MAM2_WOTS_HASH_SIZE);
  X = MAM2_TRITS_INIT(X, MAM2_WOTS_SIG_SIZE);

  prng_gen_str(p, MAM2_PRNG_DST_SECKEY, "WOTSPRNGKEY", K);
  prng_gen_str(p, 4, "WOTSNONCE", N);

  printf("mss\n");
  trits_print2("\tK\t=", K, "\n");
  printf("----------\n");

  for(i = 0; ns[i] < TEST_MAX_SIZE; ++i)
  {
    mss_init(m, r, s, w, d, n1, n2);
    mss_gen(m, pk);
    mss_sign(m, H, sig);
    mss_save(m, );

    n = trits_take(N, ns[i]);

    wots_gen_sk(w, r, n);
    wots_calc_pk(w, Q);
    trits_print2("\tn\t=", n, "\n");
    trits_print2("\tpk\t=", Q, "\n");
    printf("----------\n");

    prng_gen_str(p, 4, "WOTSHASH", H);
    wots_sign(w, H, X);
    trits_print2("\tH\t=", H, "\n");
    trits_print2("\tsig\t=", X, "\n");
    printf("----------\n");

    prng_gen_str(p, 4, "WOTSHASH9", H);
    wots_sign(w, H, X);
    trits_print2("\tH\t=", H, "\n");
    trits_print2("\tsig\t=", X, "\n");
    printf("----------\n");
  }
  printf("\n");
#undef TEST_MAX_SIZE
}

void test_gen_ntru(iprng *p, intru *n)
{
#define TEST_MAX_SIZE (MAM2_SPONGE_RATE * 3)
  MAM2_TRITS_DEF0(K, MAM2_PRNG_KEY_SIZE);
  MAM2_TRITS_DEF0(SK, MAM2_NTRU_SK_SIZE);
  MAM2_TRITS_DEF0(PK, MAM2_NTRU_PK_SIZE);
  MAM2_TRITS_DEF0(X, MAM2_NTRU_KEY_SIZE);
  MAM2_TRITS_DEF0(Y, MAM2_NTRU_EKEY_SIZE);
  MAM2_TRITS_DEF0(N, TEST_MAX_SIZE);
  trits_t x, y;
  size_t i, ns[] = { 0, 243, 487, TEST_MAX_SIZE+1 };

  K = MAM2_TRITS_INIT(K, MAM2_PRNG_KEY_SIZE);
  SK = MAM2_TRITS_INIT(SK, MAM2_NTRU_SK_SIZE);
  PK = MAM2_TRITS_INIT(PK, MAM2_NTRU_PK_SIZE);
  X = MAM2_TRITS_INIT(X, MAM2_NTRU_KEY_SIZE);
  Y = MAM2_TRITS_INIT(Y, MAM2_NTRU_EKEY_SIZE);
  N = MAM2_TRITS_INIT(N, TEST_MAX_SIZE);

  prng_gen_str(p, MAM2_PRNG_DST_SECKEY, "NTRUGENNONCE", n);
  prng_gen_str(p, MAM2_PRNG_DST_SECKEY, "NTRUENCRNONCE", n);
  prng_gen_str(p, MAM2_PRNG_DST_SECKEY, "NTRUPRNGKEY", K);
  prng_gen_str(p, MAM2_PRNG_DST_SECKEY, "NTRUKEY", X);

  prng_init(r, K);

  printf("ntru\n");
  trits_print2("\trngK\t=", K, "\n");
  printf("----------\n");

  for(i = 0; ns[i] < TEST_MAX_SIZE; ++i)
  {
    n = trits_take(N, ns[i]);

    ntru_gen(n, r, n, PK);
    trits_print2("\tn\t=", n, "\n");
    trits_print2("\tsk\t=", ntru_sk_trits(n), "\n");
    trits_print2("\tpk\t=", PK, "\n");
    printf("----------\n");

    ntru_encr(Q, r, s, X, N, Y);
    trits_print2("\tX\t=", X, "\n");
    trits_print2("\tn\t=", n, "\n");
    trits_print2("\tY\t=", Y, "\n");
    printf("----------\n");
  }
  printf("\n");
#undef TEST_MAX_SIZE
}
#endif

static isponge *test_create_sponge(ialloc *a) {
  test_sponge_t *t = mam2_alloc(a, sizeof(test_sponge_t));
  return test_sponge_init(t);
}
static void test_delete_sponge(ialloc *a, isponge *s) {
  mam2_free(a, (test_sponge_t *)s);
}

// bool_t _gen() {
//   test_sponge_t _s[1];
//   test_spongos_t _sg[1];
//   test_prng_t _p[1], _pa[1], _pb[1];
//   test_wots_t _w[1];
//   test_mss1 _m1[1];
//   test_mss2 _m2[1];
//   test_mss3 _m3[1];
//   test_mss4 _m4[1];
//   test_mss _m[1];
//   test_ntru_t _n[1];
//
//   isponge *s = test_sponge_init(_s);
//   ispongos *sg = test_spongos_init(_sg, s);
//   iprng *p = test_prng_init(_p, s);
//   iprng *pa = test_prng_init(_pa, s);
//   iprng *pb = test_prng_init(_pb, s);
//   iwots *w = test_wots_init(_w, s);
//   imss *m1 = test_mss_init1(_m1);
//   imss *m2 = test_mss_init2(_m2);
//   imss *m3 = test_mss_init3(_m3);
//   imss *m4 = test_mss_init4(_m4);
//   imss *m = test_mss_init(_m);
//   intru *n = test_ntru_init(_n);
//
// #if 0
//   test_gen_sponge(p, s);
//   test_gen_spongos(p, s);
//   test_gen_prng(p, r, s);
//   test_gen_wots(p, w, s, r);
//   test_gen_mss(p, m, r, s, w);
//   test_gen_ntru(p, n);
// #endif
//
//   return 1;
// }
//
// bool_t _() {
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
//   isponge *s = test_sponge_init(_s);
//   ispongos *sg = test_spongos_init(_sg, s);
//   iprng *p = test_prng_init(_p, s);
//   iprng *pa = test_prng_init(_pa, s);
//   iprng *pb = test_prng_init(_pb, s);
//   iwots *w = test_wots_init(_w, s);
//   imss *m1 = test_mss_init1(_m1);
//   imss *m2 = test_mss_init2(_m2);
//   imss *m3 = test_mss_init3(_m3);
//   imss *m4 = test_mss_init4(_m4);
//   imss *m42 = test_mss_init4(_m42);
//   imss *m5 = test_mss_init5(_m5);
//   imss *mx = test_mss_initx(_mx);
//   imss *m = test_mss_init(_m);
//   intru *n = test_ntru_init(_n);
//
//   bool_t r = 1, rr;
//   clock_t clk;
// #define __run_test(n, f)                                \
//   do {                                                  \
//     printf(n);                                          \
//     clk = clock();                                      \
//     rr = f;                                             \
//     clk = clock() - clk;                                \
//     printf(" \t%s \t%d clk\n", bool_str(rr), (int)clk); \
//     r = r && rr;                                        \
//   } while (0)
//
//   __run_test("Trits  ", trits_test());
//   __run_test("Sponge ", sponge_test(s));
//   __run_test("Spongos", spongos_test(sg));
// #if 0
//   __run_test("Curl   ", curl_test());
// #endif
//   __run_test("PRNG   ", prng_test(p));
//   __run_test("WOTS   ", wots_test(w, p));
//   __run_test("MSS1   ", mss_test(m1, p, sg, w, 1) && test_mss_check1(_m1));
//   __run_test("MSS2   ", mss_test(m2, p, sg, w, 2) && test_mss_check2(_m2));
//   __run_test("MSS3   ", mss_test(m3, p, sg, w, 3) && test_mss_check3(_m3));
//   __run_test("MSS4   ", mss_test(m4, p, sg, w, 4) && test_mss_check4(_m4));
//   __run_test("MSS4sto", mss_test_store(m4, m42, p, sg, w, 4) &&
//                             test_mss_check4(_m4) && test_mss_check4(_m42));
// #if 0
//   __run_test("MSS5   ", mss_test(m5, p, sg, w, 5) && test_mss_check5(_m5));
//   __run_test("MSSX   ", mss_test(mx, p, sg, w, 10) && test_mss_checkx(_mx));
// #endif
//   __run_test("MSS    ",
//              mss_test(m, p, sg, w, MAM2_MSS_TEST_MAX_D) &&
//              test_mss_check(_m));
//   __run_test("Poly   ", poly_test());
//   __run_test("NTRU   ", ntru_test(n, sg, p));
//   __run_test("PB3    ", pb3_test());
//   __run_test("MAM2   ",
//              mam2_test(s, 0, test_create_sponge, test_delete_sponge, pa,
//              pb));
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
//   bool_t r = 1;
//   size_t sponge_hash_Yn = 81;
//   size_t sponge_encr_Yn = 162;
//   size_t sponge_decr_Xn = 162;
//   size_t prng_gen_Yn = 162;
//   clock_t clk;
//
//   char sponge_hash_Y[81];
//   char sponge_encr_Y[162];
//   char sponge_decr_X[162];
//   char prng_gen_Y[162];
//   char wots_gen_sign_pk[MAM2_WOTS_PK_SIZE / 3];
//   char wots_gen_sign_sig[MAM2_WOTS_SIG_SIZE / 3];
//
//   do {
//     r = _() && r;
//     r = sponge_point_test() && r;
//     sponge_hash(162, X, sponge_hash_Yn, sponge_hash_Y);
//     sponge_encr(81, K, 162, X, sponge_encr_Yn, sponge_encr_Y);
//     sponge_decr(81, K, 162, X, sponge_decr_Xn, sponge_decr_X);
//     prng_gen(81, K, 12, N, prng_gen_Yn, prng_gen_Y);
//     wots_gen_sign(81, K, 12, N, MAM2_WOTS_PK_SIZE / 3, wots_gen_sign_pk, 78,
//     H,
//                   MAM2_WOTS_SIG_SIZE / 3, wots_gen_sign_sig);
//   } while (0)
// }
//
// int main() {
//   test();
//   return 0;
// }
