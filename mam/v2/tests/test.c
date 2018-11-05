#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unity/unity.h>

typedef int bool_t;
#define MAM2_WOTS_PK_SIZE 243
#define MAM2_WOTS_SK_PART_SIZE 162
#define MAM2_WOTS_SK_PART_COUNT 81
#define MAM2_WOTS_SK_SIZE (MAM2_WOTS_SK_PART_SIZE * MAM2_WOTS_SK_PART_COUNT)
#define MAM2_WOTS_SIG_SIZE MAM2_WOTS_SK_SIZE

#define __decl_test(f)                                                       \
  bool_t f##_();                                                             \
  void f##_sponge_hash(size_t Xn, char *X, size_t Yn, char *Y);              \
  void f##_sponge_encr(size_t Kn, char *K, size_t Xn, char *X, size_t Yn,    \
                       char *Y);                                             \
  void f##_sponge_decr(size_t Kn, char *K, size_t Yn, char *Y, size_t Xn,    \
                       char *X);                                             \
  void f##_prng_gen(size_t Kn, char *K, size_t Nn, char *N, size_t Yn,       \
                    char *Y);                                                \
  void f##_wots_gen_sign(size_t Kn, char *K, size_t Nn, char *N, size_t pkn, \
                         char *pk, size_t Hn, char *H, size_t sign, char *sig)

__decl_test(test_word_rep_int_int8_1);
#if 0
__decl_test(test_word_rep_int_int8_5);
#endif
__decl_test(test_word_rep_interleaved_uint8_6);
__decl_test(test_word_rep_interleaved_uint8_8);
__decl_test(test_word_rep_packed_uint8_3);
__decl_test(test_word_rep_packed_uint8_4);

void test() {
  char N[12 + 1] = "9ABCKNLMOXYZ";

  char K[81 + 1] =
      "ABCKNLMOXYZ99AZNODFGWERXCVH"
      "XCVHABCKNLMOXYZ99AZNODFGWER"
      "FGWERXCVHABCKNLMOXYZ99AZNOD";

  char H[78 + 1] =
      "BACKNLMOXYZ99AZNODFGWERXCVH"
      "XCVHABCKNLMOXYZ99AZNODFGWER"
      "FGWERXCVHABCKNLMOXYZ99AZ";

  char X[162 + 1] =
      "ABCKNLMOXYZ99AZNODFGWERXCVH"
      "XCVHABCKNLMOXYZ99AZNODFGWER"
      "FGWERXCVHABCKNLMOXYZ99AZNOD"
      "Z99AZNODFGABCKNLMOXYWERXCVH"
      "AZNODFGXCVKNLMOXHABCYZ99WER"
      "FGWERVHABCKXCNLM9AZNODOXYZ9";

  bool_t r = 1;
  size_t sponge_hash_Yn = 81;
  size_t sponge_encr_Yn = 162;
  size_t sponge_decr_Xn = 162;
  size_t prng_gen_Yn = 162;
  clock_t clk;

#define __run_test0(f)                                                     \
  char f##_sponge_hash_Y[81];                                              \
  char f##_sponge_encr_Y[162];                                             \
  char f##_sponge_decr_X[162];                                             \
  char f##_prng_gen_Y[162];                                                \
  char f##_wots_gen_sign_pk[MAM2_WOTS_PK_SIZE / 3];                        \
  char f##_wots_gen_sign_sig[MAM2_WOTS_SIG_SIZE / 3];                      \
  do {                                                                     \
    printf("\n %s \n", #f);                                                \
    clk = clock();                                                         \
    r = f##_() && r;                                                       \
    f##_sponge_hash(162, X, sponge_hash_Yn, f##_sponge_hash_Y);            \
    f##_sponge_encr(81, K, 162, X, sponge_encr_Yn, f##_sponge_encr_Y);     \
    f##_sponge_decr(81, K, 162, X, sponge_decr_Xn, f##_sponge_decr_X);     \
    f##_prng_gen(81, K, 12, N, prng_gen_Yn, f##_prng_gen_Y);               \
    f##_wots_gen_sign(81, K, 12, N, MAM2_WOTS_PK_SIZE / 3,                 \
                      f##_wots_gen_sign_pk, 78, H, MAM2_WOTS_SIG_SIZE / 3, \
                      f##_wots_gen_sign_sig);                              \
    clk = clock() - clk;                                                   \
    printf("\n clock %d (%d sec)\n", (int)clk, (int)clk / CLOCKS_PER_SEC); \
  } while (0)
#define __run_test(f)                                                        \
  __run_test0(f);                                                            \
  do {                                                                       \
    bool_t rr = 1;                                                           \
    rr = rr && (0 == memcmp(test_word_rep_int_int8_1_sponge_hash_Y,          \
                            f##_sponge_hash_Y, sponge_hash_Yn));             \
    rr = rr && (0 == memcmp(test_word_rep_int_int8_1_sponge_encr_Y,          \
                            f##_sponge_encr_Y, sponge_encr_Yn));             \
    rr = rr && (0 == memcmp(test_word_rep_int_int8_1_sponge_decr_X,          \
                            f##_sponge_decr_X, sponge_decr_Xn));             \
    rr = rr && (0 == memcmp(test_word_rep_int_int8_1_prng_gen_Y,             \
                            f##_prng_gen_Y, prng_gen_Yn));                   \
    rr = rr && (0 == memcmp(test_word_rep_int_int8_1_wots_gen_sign_pk,       \
                            f##_wots_gen_sign_pk, MAM2_WOTS_PK_SIZE / 3));   \
    rr = rr && (0 == memcmp(test_word_rep_int_int8_1_wots_gen_sign_sig,      \
                            f##_wots_gen_sign_sig, MAM2_WOTS_SIG_SIZE / 3)); \
    if (!rr) printf("\n !!! impl differs from reference impl.\n");           \
    r = r && rr;                                                             \
  } while (0)

  // reference implementation
  __run_test0(test_word_rep_int_int8_1);

#if 0
  __run_test(test_word_rep_int_int8_5);
#endif
  __run_test(test_word_rep_interleaved_uint8_6);
  __run_test(test_word_rep_interleaved_uint8_8);
  __run_test(test_word_rep_packed_uint8_3);
  __run_test(test_word_rep_packed_uint8_4);

#undef __run_test0
#undef __run_test
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test);

  return UNITY_END();
}