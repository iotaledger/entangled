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

  char sponge_hash_Y[81];
  char sponge_encr_Y[162];
  char sponge_decr_X[162];
  char prng_gen_Y[162];
  char wots_gen_sign_pk[MAM2_WOTS_PK_SIZE / 3];
  char wots_gen_sign_sig[MAM2_WOTS_SIG_SIZE / 3];

  return;

  sponge_hash(162, X, sponge_hash_Yn, sponge_hash_Y);
  sponge_encr(81, K, 162, X, sponge_encr_Yn, sponge_encr_Y);
  sponge_decr(81, K, 162, X, sponge_decr_Xn, sponge_decr_X);
  prng_gen(81, K, 12, N, prng_gen_Yn, prng_gen_Y);
  wots_gen_sign(81, K, 12, N, MAM2_WOTS_PK_SIZE / 3, wots_gen_sign_pk, 78, H,
                MAM2_WOTS_SIG_SIZE / 3, wots_gen_sign_sig);

  /*
   * The following test don't compare against known output but
   * rather against "test_word_rep_int_int8_1_*"

  bool_t rr = 1;
  rr = rr && (0 == memcmp(test_word_rep_int_int8_1_sponge_hash_Y, sponge_hash_Y,
                          sponge_hash_Yn));
  /*rr = rr && (0 == memcmp(test_word_rep_int_int8_1_sponge_encr_Y,
  sponge_encr_Y, sponge_encr_Yn)); rr = rr && (0 ==
  memcmp(test_word_rep_int_int8_1_sponge_decr_X, sponge_decr_X,
                          sponge_decr_Xn));
  rr = rr && (0 == memcmp(test_word_rep_int_int8_1_prng_gen_Y, prng_gen_Y,
                          prng_gen_Yn));
  rr = rr && (0 == memcmp(test_word_rep_int_int8_1_wots_gen_sign_pk,
                          wots_gen_sign_pk, MAM2_WOTS_PK_SIZE / 3));
  rr = rr && (0 == memcmp(test_word_rep_int_int8_1_wots_gen_sign_sig,
                          wots_gen_sign_sig, MAM2_WOTS_SIG_SIZE / 3));
  if (!rr) printf("\n !!! impl differs from reference impl.\n");
  r = r && rr;*/
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test);

  return UNITY_END();
}