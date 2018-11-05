#define MAM2_API static
#define MAM2_SAPI static
#define MAM2_INLINE __inline
//#define MAM2_MSS_TRAVERSAL
//#define MAM2_MSS_DEBUG
//#define MAM2_DEBUG

#include <mam2/alloc.c>
#include <mam2/buffers.c>
#include <mam2/curl.c>
#include <mam2/mam2.c>
#include <mam2/mss.c>
#include <mam2/ntru.c>
#include <mam2/pb3.c>
#include <mam2/prng.c>
#include <mam2/sponge.c>
#include <mam2/trits.c>
#include <mam2/wots.c>

MAM2_SAPI void test_f(void *buf, word_t *s) {
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

void test_name(_sponge_encr)(size_t Kn, char *K, size_t Xn, char *X, size_t Yn,
                             char *Y) {
  test_sponge _s[1];
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

void test_name(_sponge_decr)(size_t Kn, char *K, size_t Yn, char *Y, size_t Xn,
                             char *X) {
  test_sponge _s[1];
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

void test_name(_prng_gen)(size_t Kn, char *K, size_t Nn, char *N, size_t Yn,
                          char *Y) {
  test_sponge _s[1];
  test_prng _p[1];
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

void test_name(_wots_gen_sign)(size_t Kn, char *K, size_t Nn, char *N,
                               size_t pkn, char *pk, size_t Hn, char *H,
                               size_t sign, char *sig) {
  test_sponge _s[1];
  test_prng _p[1];
  test_wots _w[1];
  isponge *s = test_sponge_init(_s);
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