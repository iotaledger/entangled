/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <unity/unity.h>

#include "common/curl-p/trit.h"
#include "common/sign/v1/iss_curl.h"
#include "common/trinary/trit_tryte.h"

#define TEST_SEED                                                              \
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRSTUVWXYZ9ABCDEFGHIJKLMNOPQRST" \
  "UVWXYZ9"
#define EXPECT_SUBSEED                                                         \
  "PKKJZREHPYHNIBWAPYEXHXEAFZCI99UWZNKBOCCECFTDUXG9YGYDAGRLUBJVKMYNWPRCPYENAC" \
  "HOYSHJO"

void test_subseed(trit_t *seed, trit_t *subseed, int64_t i, Curl *curl) {
  tryte_t tryte_seed[] = TEST_SEED;
  trytes_to_trits(tryte_seed, seed, 81);

  iss_curl_subseed(seed, subseed, i, curl);

  trits_to_trytes(subseed, tryte_seed, HASH_LENGTH_TRIT);
  TEST_ASSERT_EQUAL_MEMORY(EXPECT_SUBSEED, tryte_seed, strlen(EXPECT_SUBSEED));
}
#undef EXPECT_SUBSEED
#undef TEST_SEED

#define EXP_KEY                                                                \
  "XLX9LMDATRVZOPBGTGEMCDTZHRVZDAEKXWAIWGXBQLJQURYJPW9DBYANYYMVJLGZN9GPDOGSUN" \
  "ZBJVQIBVEVJIRPYOSPZZWIKGNUBSFFSYTSJDPRHGSKWEUKXSRUVITOLMTSWTCVSLZPEVXLHYNH" \
  "I9XNUBDZWHSPSXHLEIABGEZTMTOQPJESYZGZ9DTOORQUBUITOEOWSNJPHNJQDGNXIJMVOMXAEV" \
  "CYFCXCMHRECOHDPAEHZIAMLOARATNVDCMEEYYQIFJUMXNXROWRHKKNQZRSFDHBAFCZMVBHBHNM" \
  "BNDLLMNJJF9YAO9CXOBUOIFPSVYZGCUCSXABWSENHKLLAQFJMVCWFVAMJHODNTPBYYIKVPTDSY" \
  "HUXLQ9AAPCG9ZNDMRSTWYAFSJQRV9JOTSWDUGMUCGRIIFFAUYRUYRXJHSEXZSPHFGHZSMZQHDY" \
  "S9JIVJJQKJAFEQTXCWKRMQNQFBXCWOQCMVPSVFRSYKUSWUPXTERQOTTHYQP9TCQSUQ9JVJAGQT" \
  "N9STLEVJTVDRDBWPWAIXCQX9SDWINDBBFPXYGJKWCZUGHXWURRZSKGLPLSDQMEZGZVUZLLJDV9" \
  "JGW9UZWB9GVOAZYQUCUWKYRRBSUDUQPPBDKMXSESVWHDJYFSCLBJYFKFXTJPMWJKRLPEKDKMZA" \
  "UPHMGPNNBFECGJUGX9YPTWYEDKZVZBAFJUHRGADHTYGJNXNKHKCGXYTSQ9IEQISMLKOUCZJHYI" \
  "PYYYXUXQPYFPMTOPSLIWYHHIY9ULO9FLVXKMZCDUVSFIFDRVJZGASJYYBLFDBHOTBMOHBBOIGL" \
  "LD9QCBQUMAXCXSFJXUNQOGGARHUCTAUOUGOJRQLWDQPBVENNXFQPBILXWYAKGQHWIRAHHJDJFD" \
  "MNPCVVDWHGIGWLQUSMIRG9RKEOXUIK9BCTZGDGTYESAMOFTTKCWZBRNJTTJFXZVGMKGGJJF9VI" \
  "ZISFKYS9MEUCKHBGNIPFMIRYVORSSCAUGD9RDQOL9DOBGUBOMIEGORTCSVHMJUUROUKLFCWCTO" \
  "GTDOKNEYOUCMFCGIRITYNGGAOLRDTUOJZAS9PAMYKWUJZGVHTWOIR9ILVDBUADOYW9UZQYATTA" \
  "XCYMVMNVHKJDCSMJOUNHSAAXFZUESYYVEGZRME9ALZJKSZSGANPODDZLQZGVZXCEUGHY9HNAOF" \
  "EUVZULZRTGHKOXOBXLTLCQFRVDXUBQPKMJ9ESZSQQZZWOO9LGEKJMLTVEHAKNTSWJA9TYCFL9B" \
  "PDQJYPEJJKDCOKBMYJSSRBNBMGKFQCZAHGNCSCUNNKLAFQHCZYEXFDMOAHCPDERWTSCKTPZAJG" \
  "OPGODXQET9LZLHNIRVFKOCXXZEXPJTWYOAUTMSZWVCSCTMCVCFHIQONWVO9VTUQSQYHDL9MZNP" \
  "GHPORLTIWWODWGKLMTNOL9PWIKLLEIMQRFCJDIPPC9UNEJCHROQVCFGHJYKBFQOBCLIABYTDAB" \
  "ROOLFXETOIBY9OKLJHWTKHLHUCQDRBWZUOPERCUVO9OKWZGHLCTAOPXIPBVYCYHIKYOKFTRWQP" \
  "WKRAGOVTAQFWMHUNBQAHZVE9LYIXMZYRSUELTNIHGDFUASHEUEOYSQVFOFFL9AEOTNJMC9FIAU" \
  "G99FGSBLLPAKUCUISMREANUA9ZGLCPXHHETDDSVMZGCTIZHVCITMBBRHVFHBXPNFN9IUUQVYNV" \
  "IUW9WCL9TNLIGUQJKUZSEWHPZAIPPMTEWRJRDALCDXPDCXPQFXSFCGM9YFOVJKWNS9LWOIUXSU" \
  "XCMTBESKMEDCCPW9RLHKENAUBHL9LCCGFVWUQSMMPLAWPWZSCJFIGJGY9EEYMKGOABISRZMZPY" \
  "QCQYWDFDQEGLMPECQHTQR9TIOIH9ZDKCRWHDLDAKMYBOCSXNUZZL9BPQDIYQ9SACVTUQTULDPJ" \
  "YATNOUUI9JZPIFUMILK9JFIHUZDLEAEHSWVDDO9BIMVIPCUNXWBGGRYHKYGACISUQFNDE9ZF9E" \
  "HLVXPBVABXWYNFEACIONXSVBNEBA9GZKJOGSC9LPQBXWRDX99OSZDMPEHKJKHYIPKWKFBOHDLJ" \
  "YDRIXOEAESTPRV9RWXNEFUOZARIELYRONSGLYGWJTENHKSNUHRLZWKTQHXACYICMAOR9JUPISJ" \
  "HCHHRIBVHTZRO9C9QORJKIWAYFWDIWM9ENDTSMFUS"
void test_key(trit_t *s, trit_t *k, size_t l, Curl *c) {
  iss_curl_key(s, k, l, c);
  tryte_t k_t[l];

  trits_to_trytes(k, k_t, l);

  TEST_ASSERT_EQUAL_MEMORY(EXP_KEY, k_t, l / 3 * sizeof(tryte_t));
}
#undef EXP_KEY

#define EXP_ADDY                                                               \
  "LZSSW9TZUVUJXX9F9CXTYTRWYVOB9XNANDTULZBHSUZTTJSPICWWYSBHMKZWENSFVBDCGWFTNW" \
  "LWRIYMT"

void test_addy(trit_t *k, size_t l, Curl *c) {
  tryte_t addy_trytes[l];
  addy_trytes[HASH_LENGTH_TRYTE] = 0;

  iss_curl_key_digest(k, k, l, c);
  iss_curl_address(k, k, HASH_LENGTH_TRIT, c);

  trits_to_trytes(k, addy_trytes, l);

  TEST_ASSERT_EQUAL_MEMORY(EXP_ADDY, addy_trytes, HASH_LENGTH_TRYTE * sizeof(tryte_t));
}
#undef EXP_ADDY

void test_iss() {
  const size_t key_length = ISS_KEY_LENGTH;
  trit_t seed[HASH_LENGTH_TRIT];
  trit_t subseed[HASH_LENGTH_TRIT];
  trit_t key[key_length];
  int64_t index = 0;

  Curl curl;
  curl.type = CURL_P_81;
  init_curl(&curl);

  test_subseed(seed, subseed, index, &curl);
  test_key(subseed, key, key_length, &curl);
  test_addy(key, key_length, &curl);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_iss);
  // RUN_TEST(test_curl_p_81_works);
  // RUN_TEST(test_other);

  return UNITY_END();
}
