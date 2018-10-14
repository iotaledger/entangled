/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <unity/unity.h>

#include "common/kerl/kerl.h"
#include "common/sign/v1/iss_kerl.h"
#include "common/trinary/trit_tryte.h"

#define TEST_SEED                                                              \
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRST" \
  "UVWXYZ9"
#define EXPECT_SUBSEED                                                         \
  "APSNZAPLANAGSXGZMZYCSXROJ9KUX9HVOPODQHMWNJOCGBKRIOOQKYGPFAIQBYNIODMIWMFKJG" \
  "KRWFFPY"

void test_subseed(trit_t *seed, trit_t *subseed, int64_t i, Kerl *kerl) {
  tryte_t tryte_seed[] = TEST_SEED;
  trytes_to_trits(tryte_seed, seed, 81);

  iss_kerl_subseed(seed, subseed, i, kerl);

  trits_to_trytes(subseed, tryte_seed, HASH_LENGTH_TRIT);
  //  TEST_ASSERT_EQUAL_MEMORY(EXPECT_SUBSEED, tryte_seed,
  //  strlen(EXPECT_SUBSEED));
}
#undef EXPECT_SUBSEED
#undef TEST_SEED

#define EXP_KEY                                                                \
  "UZQZYUJSSGZWQITWBXP9CVOIDVSXNDWIRIULPLOBWAJZKSVA9XJTFTBBBUZOECZ9DLMWZSUHNK" \
  "ETPYUHBHWXUEZHDXAFSPNANAXHHVMJOUUDYZGGYIJAIDTDUZOUIJFMOHCYVAUOGPWZWDGNNSJP" \
  "WMPJLXORDBHDS9KAIUSS9CCLUNBWJIGJEUGIZIYVIQMF9AKYRWS9GKKMODALTCVKCNKWEMTSSV" \
  "DKVQXXNIQCRWLQPHHXVQWSYFSEYLE9BGSPFFLMPICGX9IIYMXBGOVOWDAKMNETSDXVRR9WSVUV" \
  "WKPCLVVBFJ9DDANYHYVFOXEOJ9KWQPAPWEHUYPWY9NAREIPGULCTASQPDPTDDKCXYMMMXTBCSF" \
  "ZPYUCANTGIFWMKKWGHXGGROSLHQTFDFFGMXKSHXZEZD9JBYIHZSDZTTIJ9LUGILFNXMTONJOFI" \
  "TKNMVVVELG9DROIQC9FQIZIJFPQGA9KEGUKFTNZQZDQVVIANGZTDJEEKYDOIBOEEIDMSTHGEQM" \
  "QXECDVXULNPBIGNZQFHQVIORCPJJJZI9CNSMZBXYKVTGTHNLYRRKJZBS9NEHKOEKEVOQFIZBSB" \
  "9CNXGIIJGPZG9NVOSPWBQR9ZW9QOKFLGLZBAJFYWVRKQCIFFACSBAECDRFJQDEVZL9CSGOXQSB" \
  "I9ZTWOAITEGXIAYFGMPMQGGNPGRXNXSHXRSHPTRXOJ9ZOPNBDGIYYKB9ASMAPS9SZEFDUWE9RQ" \
  "IRQWBZQPHXDZRBIABAJZGETOKKVXYDIWBGSXJVLVPBTUFHCJNU9KJDAYTQSFBSEMYIOPQDKUXF" \
  "GVPKNKABARXOJXTIZZZDIAQWZAONZBVEIZHBXMHLFREOCFDCHSBJSJPWYZJAJBUAMPWNAWAWZG" \
  "GGCN9PGE9SDPCRZRXQGBVOKNYMUWMAGGGCZHYZMPNULQSMLDVGVYPQQSHNHCEVDWRNLWJCJRKE" \
  "SRMGCFQNPXZEFJKSOBQCEH9TMJIORW9CAZN9HOFUQSLLDJVSQGZBLZ9RIUHDGNOSWCKPOGMSLX" \
  "XXWHQZ9V9PSPY9YNDGHGYURKZOYCGQEUUPNRLGDADAXLRVRVDBU9BHJGBMBLNSNKITRDXREYRG" \
  "MVZBQHKU9RMFPFMQLYQGINQWIAHDYELYUKNAQWBVBIOFKWEQFQQHJXENY9ELRSMDWPPYVPQCDB" \
  "CDUHUVIQPVIV9L9KZAOYEIFTDBCPFWDGRVE99QH9RKOGBSLNLTHYNCLCKRXTUZRVZMU9QOOSCS" \
  "Z9MSVROW9EFBSAYJAZLKOLVBTMEZYCFJLHUKDYJVIOUSMYUKPADUKHIACZYZ9XACEMQNWSHZCX" \
  "CMCLPIAEXLJTAVSORWCFZGJYZJUCCBJLJUXGKNNFBBVAW9WOPH9IFBHHZSMHJVQTQXURMSHUXU" \
  "WJBARENYAFRKQLMQCFNGIGLBDRNIDEUSLTGA9REENKUTKAAP9US9BLWG9FBCUU9OFZIHTQRGND" \
  "VVABMDOCRPLDASHSBLVYIYIW9YDZRBAQSMTAZOOIJITZQSOPNUMEPJSFSPWHHBDNRYNVRIQEEO" \
  "O9TNCQDJHZBEUSQEJQRB9IBIBFOJSGJHUURWKWVVERMQLLELU9UYMCXXVJZTLRWQT99VPRFUZZ" \
  "ICUVCPEPGITNGWEQPKP9YYT9SRJYBLDMN9PVEKEXCSNIUIOEJLUY9V9IIAPN9SFMXGWLCMGCWN" \
  "WBLI9G9APIHZSZPDZXLKOJTPAXYNIOU9QHYFZRJXACGJQQD9DOKUTSLKZLOLGHVKHAXERIQ9HD" \
  "UFLLW9DRPNREVEO9QTQAUSXJLZOGSKSBLCXCFBGUUOUEYTYOJSFN9ZWVMIUADBRANUKNB9PHYU" \
  "JX9HZUYWRNUFCJVSXBMCWJDDPIPXCDZV9YHHYNXQUIENVVXUXS9HKAPI99GWIVF9BHOIORKDDI" \
  "NLLLATHXHMTEKZVHWRWDKKNUGPZC9AVPEOGISOLGUNOZKYHZZZGXKUJABSNSPOHDACQMKBEFVB" \
  "IESKLYJWLUOKQSTGVQWFLMWGJVXNB9IQRXLMDAFWRHZFTCPUNWTN9IJDRSZMOOOROXZGLYAHRW" \
  "XYJ9DXEUVCXIJELVGD9ACQRJEVSLGVUD9YLSWBHGANGIFSHAGTXDUFEAQPFQSXQADOPXUMSBDO" \
  "KCNXJAO9HRAS9GAMINAEMLJPQTCFNSLUWPRWHEDTY"
void test_key(trit_t *s, trit_t *k, size_t l, Kerl *c) {
  iss_kerl_key(s, k, l, c);
  tryte_t k_t[l];

  trits_to_trytes(k, k_t, l);

  TEST_ASSERT_EQUAL_MEMORY(EXP_KEY, k_t, l / 3 * sizeof(tryte_t));
}
#undef EXP_KEY

#define EXP_ADDY                                                               \
  "CFOYUCXLHLSUBAEYOTAWUNRPJFA9TSJNLBFLMZQASPTVCMTFBOQQRGGQ9MRZCJWYGBORJZQWVS" \
  "BLVKBVW"

void test_addy(trit_t *k, size_t l, Kerl *c) {
  tryte_t addy_trytes[l];

  iss_kerl_key_digest(k, k, l, c);
  iss_kerl_address(k, k, HASH_LENGTH_TRIT, c);

  trits_to_trytes(k, addy_trytes, l);

  addy_trytes[HASH_LENGTH_TRYTE] = 0;

  TEST_ASSERT_EQUAL_MEMORY(EXP_ADDY, addy_trytes,
                           HASH_LENGTH_TRYTE * sizeof(tryte_t));
}
#undef EXP_ADDY

void test_iss() {
  const size_t key_length = ISS_KEY_LENGTH;
  trit_t seed[HASH_LENGTH_TRIT];
  trit_t subseed[HASH_LENGTH_TRIT];
  trit_t key[key_length];
  int64_t index = 0;

  Kerl kerl;
  init_kerl(&kerl);

  test_subseed(seed, subseed, index, &kerl);
  test_key(subseed, key, key_length, &kerl);
  test_addy(key, key_length, &kerl);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_iss);

  return UNITY_END();
}
