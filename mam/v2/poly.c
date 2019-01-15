/*!
\file poly.c
\brief MAM2 NTRU polynomials arithmetic.
*/
#include "poly.h"
#include "poly_param.h"

/*!
\brief γ²ⁿ ≡ γ γ⁻¹ ≡ 1 (mod q).
\brief ωⁿ  ≡ ω ω⁻¹ ≡ 1 (mod q). ω ≡ γ² (mod q).
\brief n n⁻¹ ≡ 1 (mod q).
*/
#if defined(MAM2_POLY_MRED_BINARY)
#define MAM2_POLY_COEFF_GAMMA ((poly_coeff_t)4059)
#define MAM2_POLY_COEFF_GAMMA_INV ((poly_coeff_t)2340)
#define MAM2_POLY_COEFF_OMEGA ((poly_coeff_t)3835)
#define MAM2_POLY_COEFF_OMEGA_INV ((poly_coeff_t)5601)
#define MAM2_POLY_COEFF_N ((poly_coeff_t)10924)
#define MAM2_POLY_COEFF_N_INV ((poly_coeff_t)64)
#define MAM2_POLY_COEFF_ONE ((poly_coeff_t)4091)
#define MAM2_POLY_COEFF_MINUS_ONE \
  ((poly_coeff_t)MAM2_POLY_Q - MAM2_POLY_COEFF_ONE)

/*! R = 2¹⁶. */
#define MAM2_POLY_MRED_R_LOG 16
/*! R⁻¹ = 9 2⁸ ≡ 2304 (mod q). */
#define MAM2_POLY_MRED_RI 2304
/*! q⁻¹ q - R R⁻¹ = 1 */
#define MAM2_POLY_MRED_Q_INV 12287
#else
#define MAM2_POLY_COEFF_GAMMA ((poly_coeff_t)7)
#define MAM2_POLY_COEFF_GAMMA_INV ((poly_coeff_t)-3511)
#define MAM2_POLY_COEFF_OMEGA ((poly_coeff_t)49)
#define MAM2_POLY_COEFF_OMEGA_INV ((poly_coeff_t)1254)
#define MAM2_POLY_COEFF_N ((poly_coeff_t)1024)
#define MAM2_POLY_COEFF_N_INV ((poly_coeff_t)-12)
#define MAM2_POLY_COEFF_ONE ((poly_coeff_t)1)
#define MAM2_POLY_COEFF_MINUS_ONE ((poly_coeff_t)-1)
#endif

#if defined(MAM2_POLY_MRED_BINARY)
static poly_coeff_t poly_coeff_mredd(poly_dcoeff_t m) {
  poly_coeff_t s;
  s = m;

  s = (poly_coeff_t)m * MAM2_POLY_MRED_Q_INV;
  /*s = (s << 12) + (s << 13) - s;*/

  m = m + s * MAM2_POLY_Q;
  /*m = m + (s << 12) + (s << 13) + s;*/

  /* s := m div R */
  s = m >> MAM2_POLY_MRED_R_LOG;
  s = (s < MAM2_POLY_Q) ? s : (s - MAM2_POLY_Q);
  MAM2_ASSERT(s < MAM2_POLY_Q);
  return s;
}
#endif

/*!
\brief Convert integer into internal polynomial coefficient representation.
\note The input integer must be within the range [-(Q-1)/2,...,(Q-1)/2].
*/
static poly_coeff_t poly_coeff_from_trint9(trint9_t t) {
#if defined(MAM2_POLY_MRED_BINARY)
  /* `c*R (mod q)` */
  poly_dcoeff_t d;
  poly_coeff_t c;
  MAM2_ASSERT(-(MAM2_POLY_Q - 1) / 2 <= t && t <= (MAM2_POLY_Q - 1) / 2);

  d = t < 0 ? (poly_dcoeff_t)t + MAM2_POLY_Q : t;
  c = (d << MAM2_POLY_MRED_R_LOG) % MAM2_POLY_Q;
  return c;
#else
  MAM2_ASSERT(-(MAM2_POLY_Q - 1) / 2 <= t && t <= (MAM2_POLY_Q - 1) / 2);
  return (poly_coeff_t)t;
#endif
}

/*!
\brief Convert internal polynomial coefficient representation into an integer.
\note The output integer will be within the range [-(Q-1)/2,...,(Q-1)/2].
*/
static trint9_t poly_coeff_to_trint9(poly_coeff_t c) {
#if defined(MAM2_POLY_MRED_BINARY)
  /* `c/R (mods q)` */
  poly_dcoeff_t d = c;
  d *= MAM2_POLY_MRED_RI;
  c = d % (poly_coeff_t)MAM2_POLY_Q;
  trint9_t t = (MAM2_POLY_Q - 1) / 2 < c ? (trint9_t)c - MAM2_POLY_Q : c;
  return t;
#else
  c = (MAM2_POLY_Q - 1) / 2 < c ? c - MAM2_POLY_Q : c;
  return c;
#endif
}

static poly_coeff_t poly_coeff_from_trint1(trint1_t t) {
  poly_coeff_t c = 0;
  MAM2_ASSERT_TRINT1(t);
  if (0 < t)
    c = MAM2_POLY_COEFF_ONE;
  else if (t < 0)
    c = MAM2_POLY_COEFF_MINUS_ONE;
  else
    ;
  return c;
}

/*! a + b (mods q) */
static poly_coeff_t poly_coeff_add(poly_coeff_t a, poly_coeff_t b) {
#if defined(MAM2_POLY_MRED_BINARY)
  /* u = a + b mod R */
  poly_coeff_t c = a + b;
  c = c < MAM2_POLY_Q ? c : c - MAM2_POLY_Q;
  return c;
#else
  /* `a+b` can overflow, so need double precision */
  poly_dcoeff_t sum = (poly_dcoeff_t)a + b;
  if (sum < -((MAM2_POLY_Q - 1) / 2))
    sum += MAM2_POLY_Q;
  else if (sum > ((MAM2_POLY_Q - 1) / 2))
    sum -= MAM2_POLY_Q;
  else
    ;
  return sum;
#endif
}

/*! a - b (mods q) */
static poly_coeff_t poly_coeff_sub(poly_coeff_t a, poly_coeff_t b) {
#if defined(MAM2_POLY_MRED_BINARY)
  /* u = a - b mod R */
  poly_coeff_t c = a < b ? MAM2_POLY_Q + a - b : a - b;
  return c;
#else
  return poly_coeff_add(a, -b);
#endif
}

/*! a * b (mods q) */
static poly_coeff_t poly_coeff_mul(poly_coeff_t a, poly_coeff_t b) {
#if defined(MAM2_POLY_MRED_BINARY)
  return poly_coeff_mredd((poly_dcoeff_t)a * b);
#else
  poly_dcoeff_t prod = (poly_dcoeff_t)a * b;
  prod = MAM2_MODS(prod, ((trint18_t)MAM2_POLY_Q * (trint18_t)MAM2_POLY_Q),
                   MAM2_POLY_Q);
  return prod;
#endif
}

/*! a * b + c (mods q) */
static poly_coeff_t poly_coeff_mul_add(poly_coeff_t a, poly_coeff_t b,
                                       poly_coeff_t c) {
#if defined(MAM2_POLY_MRED_BINARY)
  return poly_coeff_add(poly_coeff_mul(a, b), c);
#else
  poly_dcoeff_t prod = (poly_dcoeff_t)a * b + c;
  prod = MAM2_MODS(prod, ((trint18_t)MAM2_POLY_Q * (trint18_t)MAM2_POLY_Q),
                   MAM2_POLY_Q);
  return prod;
#endif
}

static poly_coeff_t poly_coeff_inv(poly_coeff_t a) {
  poly_coeff_t e, t;
  size_t i;
  MAM2_ASSERT(MAM2_POLY_Q == ((3 * (1 << 12)) + 1));
  /* q-2 = 10111111111111b */

  e = a;
  for (i = 0; i < 11; ++i) {
    t = poly_coeff_mul(e, e);
    e = poly_coeff_mul(t, a);
  }

  t = poly_coeff_mul(e, a);
  t = poly_coeff_mul(t, t);
  e = poly_coeff_mul(t, e);

  MAM2_ASSERT(poly_coeff_mul(e, a) == MAM2_POLY_COEFF_ONE);
  return e;
}

#if 0
static void poly_coeff_exp_table(poly_coeff_t u, size_t n, poly_coeff_t *t)
{
  size_t i;
  poly_coeff_t c = MAM2_POLY_COEFF_ONE;
  for(i = 0; i < n; ++i,
      c = poly_coeff_mul(c, u))
    t[i] = c;
}

#include <stdio.h>
 void poly_gen_param()
{
  size_t i, j, k;
  FILE *f;

  for(i = 0; i < MAM2_POLY_N; ++i)
  {
    k = 0;
    for(j = 0; j < MAM2_POLY_N_LOG; ++j)
      if(i & (1 << j))
        k |= 1 << (MAM2_POLY_N_LOG - 1 - j);
    poly_idx_rev[i] = k;
  }

  poly_coeff_exp_table(
    MAM2_POLY_COEFF_GAMMA,
    2*MAM2_POLY_N+1,
    poly_gamma_exp);

  if(f = fopen("poly_idx_rev.c", "w"))
  {
    fprintf(f, "static size_t poly_idx_rev[MAM2_POLY_N] = {");
    for(i = 0; i < MAM2_POLY_N; ++i)
    {
      if(0 == i % 16)
        fprintf(f, "\n  ");
      fprintf(f, "0x%03x, ", poly_idx_rev[i]);
    }
    fprintf(f, "};\n");
    fclose(f);
  }

  if(f = fopen("poly_gamma_exp.c", "w"))
  {
    fprintf(f, "static poly_coeff_t poly_gamma_exp[2*MAM2_POLY_N+1] = {");
    for(i = 0; i <= 2*MAM2_POLY_N; ++i)
    {
      if(0 == i % 16)
        fprintf(f, "\n  ");
      fprintf(f, "%d, ", poly_gamma_exp[i]);
    }
    fprintf(f, "};\n");
    fclose(f);
  }
}
#endif

void poly_round_small(poly_t f, poly_t h) {
  size_t i;
  trint9_t c;
  for (i = 0; i != MAM2_POLY_N; ++i) {
    c = poly_coeff_to_trint9(f[i]);
    h[i] = poly_coeff_from_trint1(MAM2_MODS(c, 27 * 27 * 27, 3));
  }
}

void poly_small3_add1(poly_t f) {
#if !defined(MAM2_POLY_MRED_BINARY)
  MAM2_ASSERT(-3 <= f[0] && f[0] <= 3);
#endif
  f[0] += MAM2_POLY_COEFF_ONE;
}

void poly_small_mul3(poly_t f, poly_t h) {
  size_t i;
  poly_coeff_t c;
  for (i = 0; i < MAM2_POLY_N; ++i) {
    c = poly_coeff_add(f[i], f[i]);
    h[i] = poly_coeff_add(c, f[i]);
  }
}

void poly_add(poly_t f, poly_t g, poly_t h) {
  size_t i;
  for (i = 0; i < MAM2_POLY_N; ++i) h[i] = poly_coeff_add(f[i], g[i]);
}

void poly_sub(poly_t f, poly_t g, poly_t h) {
  size_t i;
  for (i = 0; i < MAM2_POLY_N; ++i) h[i] = poly_coeff_sub(f[i], g[i]);
}

void poly_conv(poly_t tf, poly_t tg, poly_t th) {
  size_t i;
  for (i = 0; i < MAM2_POLY_N; ++i) th[i] = poly_coeff_mul(tf[i], tg[i]);
}

bool_t poly_has_inv(poly_t t) {
  bool_t r = 1;
  size_t i;
  for (i = 0; r && i < MAM2_POLY_N; ++i) r = (0 != t[i]) ? 1 : 0;
  return r;
}

void poly_inv(poly_t tf, poly_t th) {
  size_t i;
  for (i = 0; i < MAM2_POLY_N; ++i) th[i] = poly_coeff_inv(tf[i]);
}

/* \brief tⱼ = f(γ²ʲ⁺¹) ≡ Σᵢfᵢγ⁽²ʲ⁺¹⁾ⁱ */
void poly_ntt(poly_t f, poly_t t) {
  poly_t u;
  poly_coeff_t c, d;
  size_t i, j, r;

  for (i = 0; i < MAM2_POLY_N; ++i)
    u[poly_idx_rev[i]] = poly_coeff_mul(poly_gamma_exp[i], f[i]);

  for (i = MAM2_POLY_N_LOG; i--;) {
    for (j = 0; j < MAM2_POLY_N / 2; ++j) {
      r = j & ~(((size_t)1 << i) - 1);
      /*r = j - (j % (1 << i));*/
      c = u[j + j];
      d = poly_coeff_mul(u[j + j + 1], poly_gamma_exp[r + r]);
      t[j] = poly_coeff_add(c, d);
      t[j + MAM2_POLY_N / 2] = poly_coeff_sub(c, d);
    }
    for (j = 0; j < MAM2_POLY_N; ++j) u[j] = t[j];
  }
}

/*
\brief fₖ = γ⁻ᵏn⁻¹ t(γ⁻²ᵏ)
\note
t(γ⁻²ᵏ)
 ≡ Σⱼtⱼγ⁻²ᵏʲ
 ≡ Σⱼ(Σᵢfᵢγ⁽²ʲ⁺¹⁾ⁱ)γ⁻²ᵏʲ
 ≡ Σᵢfᵢ(Σⱼγ⁽²ʲ⁺¹⁾ⁱ⁻²ᵏʲ)
 ≡ Σᵢfᵢγⁱ(Σⱼγ²ʲ⁽ⁱ⁻ᵏ⁾)
 ≡ fₖγᵏn
*/
void poly_intt(poly_t t, poly_t f) {
  poly_t u;
  poly_coeff_t c, d;
  size_t i, j, r;

  for (i = 0; i < MAM2_POLY_N; ++i) u[poly_idx_rev[i]] = t[i];

  for (i = MAM2_POLY_N_LOG; i--;) {
    for (j = 0; j < MAM2_POLY_N / 2; ++j) {
      r = j & ~(((size_t)1 << i) - 1);
      /*r = j - (j % (1 << i));*/
      c = u[j + j];
      d = poly_coeff_mul(u[j + j + 1],
                         poly_gamma_exp[2 * MAM2_POLY_N - (r + r)]);
      f[j] = poly_coeff_add(c, d);
      f[j + MAM2_POLY_N / 2] = poly_coeff_sub(c, d);
    }
    for (j = 0; j < MAM2_POLY_N; ++j) u[j] = f[j];
  }

  for (i = 0; i < MAM2_POLY_N; ++i) {
    /* TODO: precomp γ⁻ⁱn⁻¹? */
    c = poly_coeff_mul(MAM2_POLY_COEFF_N_INV,
                       poly_gamma_exp[2 * MAM2_POLY_N - i]);
    f[i] = poly_coeff_mul(c, f[i]);
  }
}

void poly_round_to_trits(poly_t f, trits_t t) {
  size_t i;
  trint9_t c;
  MAM2_ASSERT(trits_size(t) == MAM2_POLY_N);

  for (i = 0; i != MAM2_POLY_N; ++i, t = trits_drop(t, 1)) {
    c = poly_coeff_to_trint9(f[i]);
    c = MAM2_MODS(c, 27 * 27 * 27, 3);
    trits_put1(t, (trit_t)c);
  }
}

void poly_small_from_trits(poly_t f, trits_t t) {
  size_t i;
  MAM2_ASSERT(trits_size(t) == MAM2_POLY_N);

  for (i = 0; i < MAM2_POLY_N; ++i, t = trits_drop(t, 1))
    f[i] = poly_coeff_from_trint1(trits_get1(t));
}

bool_t poly_from_trits(poly_t f, trits_t t) {
  size_t i;
  trint9_t c;
  MAM2_ASSERT(trits_size(t) == 9 * MAM2_POLY_N);

  for (i = 0; i < MAM2_POLY_N; ++i, t = trits_drop(t, 9)) {
    c = trits_get9(t);
    if ((c < -((MAM2_POLY_Q - 1) / 2)) || (((MAM2_POLY_Q - 1) / 2) < c)) break;
    f[i] = poly_coeff_from_trint9(c);
  }
  return (i == MAM2_POLY_N) ? 1 : 0;
}

void poly_to_trits(poly_t f, trits_t t) {
  size_t i;
  MAM2_ASSERT(trits_size(t) == 9 * MAM2_POLY_N);

  for (i = 0; i < MAM2_POLY_N; ++i, t = trits_drop(t, 9))
    trits_put9(t, poly_coeff_to_trint9(f[i]));
}

#include <stdio.h>

void poly_print(char const *s, poly_t f) {
  size_t i;
  if (s) printf("%s", s);
  for (i = 0; i != 16 /*MAM2_POLY_N*/; ++i) {
    if (0 == (i % 16)) printf("\n");
    printf("%d ", (int)poly_coeff_to_trint9(f[i]));
  }
  printf("\n");
}

static bool_t poly_is_one(poly_t h) {
  size_t i;
  for (i = 1; i < MAM2_POLY_N; ++i)
    if (h[i] != 0) break;
  return (MAM2_POLY_COEFF_ONE == h[0]) && (i == MAM2_POLY_N) ? 1 : 0;
}

static bool_t poly_is_eq(poly_t f, poly_t g) {
  bool_t r = 1;
  size_t i;
  for (i = 0; r && i < MAM2_POLY_N; ++i) r = (f[i] == g[i]) ? 1 : 0;
  return r;
}

static void poly_mul(poly_t f, poly_t g, poly_t h) {
  typedef trint9_t poly2_t[2 * MAM2_POLY_N - 1];
  poly2_t fg;
  size_t i, j;

  for (i = 0; i < 2 * MAM2_POLY_N - 1; ++i) fg[i] = 0;

  for (i = 0; i < MAM2_POLY_N; ++i)
    for (j = 0; j < MAM2_POLY_N; ++j)
      fg[i + j] = poly_coeff_mul_add(f[i], g[j], fg[i + j]);

  for (i = 0; i < MAM2_POLY_N - 1; ++i)
    h[i] = poly_coeff_sub(fg[i], fg[i + MAM2_POLY_N]);
  h[i] = fg[i];
}

static poly_coeff_t poly_eval(poly_t f, poly_coeff_t x) {
  poly_coeff_t r = 0;
  size_t i;
  for (i = MAM2_POLY_N; i--;) r = poly_coeff_mul_add(x, r, f[i]);
  return r;
}

static void poly_ntt2(poly_t f, poly_t t) {
  size_t i;
  for (i = 0; i < MAM2_POLY_N; ++i)
    t[i] = poly_eval(f, poly_gamma_exp[i + i + 1]);
}

static void poly_intt2(poly_t t, poly_t f) {
  size_t i;
  for (i = 0; i < MAM2_POLY_N; ++i)
    f[i] =
        poly_coeff_mul(poly_coeff_mul(MAM2_POLY_COEFF_N_INV,
                                      poly_gamma_exp[2 * MAM2_POLY_N - i]),
                       poly_eval(t, poly_gamma_exp[2 * MAM2_POLY_N - (i + i)]));
}

static size_t poly_coeff_order(poly_coeff_t u, poly_coeff_t *ui) {
  size_t i;
  poly_coeff_t v;

  *ui = MAM2_POLY_COEFF_ONE;
  for (i = 0; i++ < MAM2_POLY_Q; *ui = v)
    if (MAM2_POLY_COEFF_ONE == (v = poly_coeff_mul(*ui, u)))
      /*MAM2_ASSERT(poly_coeff_inv(u) == *ui);*/
      return i;

  return 0;
}

static trint9_t poly_norm(poly_t f) {
  size_t i;
  trint9_t t, n = 0;
  for (i = 0; i != MAM2_POLY_N; ++i) {
    t = poly_coeff_to_trint9(f[i]);
    t = t < 0 ? -t : t;
    n = n < t ? t : n;
  }
  return n;
}

bool_t poly_test_ntt() {
  bool_t r = 1;
  size_t i, k, ki;
  poly_t f, g, fg, tf, tg, tfg, itf, itg, itfg, tif, itif, fitif, t2, it2;
  poly_coeff_t c = 7, a = 13, n = MAM2_POLY_N, two = 1;

  for (i = 0; i < MAM2_POLY_N; ++i, two = poly_coeff_mul(two, 2))
    f[i] = 0,     /*two,*/
        g[i] = 0; /*poly_coeff_add(two, 1233);*/

  f[0] = MAM2_POLY_COEFF_ONE; /* f(x) = 1 */
  g[1] = MAM2_POLY_COEFF_ONE; /* g(x) = x */

  for (k = ki = 0; r && k < 100; ++k) {
    poly_ntt2(f, t2);
    poly_intt2(t2, it2);
    poly_ntt(f, tf);
    poly_intt(tf, itf);
    MAM2_ASSERT(poly_is_eq(f, itf));

    poly_ntt2(g, t2);
    poly_intt2(t2, it2);
    poly_ntt(g, tg);
    poly_intt(tg, itg);
    MAM2_ASSERT(poly_is_eq(g, itg));

    poly_mul(f, g, fg);
    poly_ntt(g, tg);
    poly_conv(tf, tg, tfg);
    poly_intt(tfg, itfg);
    MAM2_ASSERT(poly_is_eq(fg, itfg));

    if (poly_has_inv(tf)) {
      ++ki;
      poly_inv(tf, tif);
      poly_intt(tif, itif);
      poly_mul(f, itif, fitif);
      MAM2_ASSERT(poly_is_one(fitif));
    }

    for (i = 0; i < MAM2_POLY_N; ++i)
      f[i] = poly_coeff_add(a, poly_coeff_mul(c, f[i]));
  }

  dbg_printf("k=%d\tki=%d\n", k, ki);
  return r;
}

bool_t poly_test_coeff_mul() {
  bool_t r = 1;
  size_t i, j;
  poly_coeff_t a, b, m;
  trint9_t t, n;

  for (i = 0; i <= MAM2_POLY_Q / 2; ++i)
    for (j = 0; j <= MAM2_POLY_Q / 2; ++j) {
      a = poly_coeff_from_trint9((trint9_t)i);
      b = poly_coeff_from_trint9((trint9_t)j);
      m = poly_coeff_mul(a, b);
      n = poly_coeff_to_trint9(m);
      t = (i * j) % MAM2_POLY_Q;
      t = t <= MAM2_POLY_Q / 2 ? t : t - MAM2_POLY_Q;
      MAM2_ASSERT(n == t);

      a = poly_coeff_from_trint9(-(trint9_t)i);
      b = poly_coeff_from_trint9((trint9_t)j);
      m = poly_coeff_mul(a, b);
      n = poly_coeff_to_trint9(m);
      t = (i * j) % MAM2_POLY_Q;
      t = t <= MAM2_POLY_Q / 2 ? -t : MAM2_POLY_Q - t;
      MAM2_ASSERT(n == t);

      a = poly_coeff_from_trint9((trint9_t)i);
      b = poly_coeff_from_trint9(-(trint9_t)j);
      m = poly_coeff_mul(a, b);
      n = poly_coeff_to_trint9(m);
      t = (i * j) % MAM2_POLY_Q;
      t = t <= MAM2_POLY_Q / 2 ? -t : MAM2_POLY_Q - t;
      MAM2_ASSERT(n == t);

      a = poly_coeff_from_trint9(-(trint9_t)i);
      b = poly_coeff_from_trint9(-(trint9_t)j);
      m = poly_coeff_mul(a, b);
      n = poly_coeff_to_trint9(m);
      t = (i * j) % MAM2_POLY_Q;
      t = t <= MAM2_POLY_Q / 2 ? t : t - MAM2_POLY_Q;
      MAM2_ASSERT(n == t);
    }
  return r;
}

bool_t poly_test_coeff() {
  bool_t r = 1;
  size_t i;
#define MAM2_POLY_TEST_N 201
  trint9_t tt[MAM2_POLY_TEST_N];

  size_t ia, ib, ic;
  poly_coeff_t a, b, c, m;
  trint9_t t;
  trint18_t s;

  for (i = 0; i < MAM2_POLY_TEST_N / 4; ++i)
    tt[i] = (trint9_t)(-(MAM2_POLY_Q - 1) / 2 + i);
  for (; i < MAM2_POLY_TEST_N / 2; ++i)
    tt[i] = (trint9_t)(MAM2_POLY_TEST_N / 2 - i);
  tt[i++] = 0;
  for (; i < MAM2_POLY_TEST_N; ++i) tt[i] = -tt[MAM2_POLY_TEST_N - 1 - i];

  for (ia = 0; ia < MAM2_POLY_TEST_N; ++ia) {
    a = poly_coeff_from_trint9(tt[ia]);
    t = poly_coeff_to_trint9(a);
    r = r && (tt[ia] == t);
    MAM2_ASSERT(r = r);

    for (ib = 0; ib < MAM2_POLY_TEST_N; ++ib) {
      b = poly_coeff_from_trint9(tt[ib]);

      m = poly_coeff_add(a, b);
      t = poly_coeff_to_trint9(m);
      s = (trint18_t)tt[ia] + tt[ib];
      s = MAM2_MODS(s, (MAM2_POLY_Q * MAM2_POLY_Q), MAM2_POLY_Q);
      r = r && t == s;
      MAM2_ASSERT(r = r);

      m = poly_coeff_sub(a, b);
      t = poly_coeff_to_trint9(m);
      s = (trint18_t)tt[ia] - tt[ib];
      s = MAM2_MODS(s, (MAM2_POLY_Q * MAM2_POLY_Q), MAM2_POLY_Q);
      r = r && t == s;
      MAM2_ASSERT(r = r);

      m = poly_coeff_mul(a, b);
      t = poly_coeff_to_trint9(m);
      s = (trint18_t)tt[ia] * tt[ib];
      s = MAM2_MODS(s, (MAM2_POLY_Q * MAM2_POLY_Q), MAM2_POLY_Q);
      r = r && t == s;
      MAM2_ASSERT(r = r);

      for (ic = 0; ic < MAM2_POLY_TEST_N; ++ic) {
        c = poly_coeff_from_trint9(tt[ic]);
        m = poly_coeff_mul_add(a, b, c);
        t = poly_coeff_to_trint9(m);
        s = (trint18_t)tt[ia] * tt[ib] + tt[ic];
        s = MAM2_MODS(s, (MAM2_POLY_Q * MAM2_POLY_Q), MAM2_POLY_Q);
        r = r && t == s;
        MAM2_ASSERT(r = r);
      }
    }
  }
#undef MAM2_POLY_TEST_N
  return r;
}

bool_t poly_test_ntrufail() {
  MAM2_POLY_DEF(f);
  MAM2_POLY_DEF(g);
  MAM2_POLY_DEF(h);
  MAM2_POLY_DEF(t);
  MAM2_POLY_DEF(s);
  MAM2_TRITS_DEF0(r, 1024);
  r = MAM2_TRITS_INIT(r, 1024);

  trits_set1(r, 1);
  trits_set1(trits_drop(r, 512), -1);
  poly_small_from_trits(f, r);
  poly_small_from_trits(g, r);

  /* NTT(1+3f) */
  poly_small_mul3(f, f);
  poly_small3_add1(f);
  poly_ntt(f, f);
  MAM2_ASSERT(poly_has_inv(f));

  /* NTT(3g) */
  poly_small_mul3(g, g);
  poly_ntt(g, g);
  MAM2_ASSERT(poly_has_inv(g));

  /* NTT(1+x+x^2..)*/
  trits_set1(r, 1);
  poly_small_from_trits(t, r);
  poly_ntt(t, t);

  /* (3g + 1+3f)*(1+x+x^2+..) */
  poly_conv(f, t, h);
  poly_intt(h, s);
  MAM2_ASSERT(3 * 1024 + 1 == poly_coeff_to_trint9(s[511]));
  poly_conv(g, t, h);
  poly_intt(h, h);
  MAM2_ASSERT(3 * 1024 == poly_coeff_to_trint9(h[511]));
  poly_add(h, s, s);
  MAM2_ASSERT(-6 * 1024 == poly_coeff_to_trint9(s[511]));

  return 1;
}

bool_t poly_test_param() {
  bool_t r = 1;
  size_t o, i;
  poly_coeff_t y, yi, w, wi, n, ni, e;

  e = poly_coeff_from_trint9(1);
  r = r && (MAM2_POLY_COEFF_ONE == e);

  y = MAM2_POLY_COEFF_GAMMA;
  o = poly_coeff_order(y, &yi);
  r = r && (o == 2 * MAM2_POLY_N);
  r = r && (yi = MAM2_POLY_COEFF_GAMMA_INV);
  r = r && (MAM2_POLY_COEFF_ONE == poly_coeff_mul(y, yi));

  w = MAM2_POLY_COEFF_OMEGA;
  o = poly_coeff_order(w, &wi);
  r = r && (o == MAM2_POLY_N);
  r = r && (wi == MAM2_POLY_COEFF_OMEGA_INV);
  r = r && (MAM2_POLY_COEFF_ONE == poly_coeff_mul(w, wi));

  n = MAM2_POLY_COEFF_N;
  o = poly_coeff_order(n, &ni);
  r = r && (ni == MAM2_POLY_COEFF_N_INV);
  r = r && (MAM2_POLY_COEFF_ONE == poly_coeff_mul(n, ni));

  for (i = 0; i <= 2 * MAM2_POLY_N; ++i)
    r = r && (MAM2_POLY_COEFF_ONE ==
              poly_coeff_mul(poly_gamma_exp[i],
                             poly_gamma_exp[2 * MAM2_POLY_N - i]));

  return r;
}

bool_t poly_test() {
  bool_t r = 1;

  r = r && poly_test_ntrufail();
  r = r && poly_test_param();

  r = r && poly_test_coeff_mul();
  r = r && poly_test_coeff();
  r = r && poly_test_ntt();
  return r;
}
