
/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/buffers.h"
#include "mam/v2/curl.h"
#include "mam/v2/mam.h"
#include "mam/v2/mss.h"
#include "mam/v2/ntru.h"
#include "mam/v2/pb3.h"
#include "mam/v2/prng.h"
#include "mam/v2/sponge.h"
#include "mam/v2/trits.h"
#include "mam/v2/wots.h"

#include <string.h>
#include <unity/unity.h>

#include <memory.h>
#include <stdio.h>

void curl_test() {
  // test value from com.iota.iri.hash.CurlTest
  char data_str[2673 + 1] =
      "RSWWSFXPQJUBJROQBRQZWZXZJWM"
      "UBVIVMHPPTYSNW9YQIQQF9RCSJJ"
      "CVZG9ZWITXNCSBBDHEEKDRBHVTW"
      "CZ9SZOOZHVBPCQNPKTWFNZAWGCZ"
      "9QDIMKRVINMIRZBPKRKQAIPGOHB"
      "THTGYXTBJLSURDSPEOJ9UKJECUK"
      "CCPVIQQHDUYKVKISCEIEGVOQWRB"
      "AYXWGSJUTEVG9RPQLPTKYCRAJ9Y"
      "NCUMDVDYDQCKRJOAPXCSUDAJGET"
      "ALJINHEVNAARIPONBWXUOQUFGNO"
      "CUSSLYWKOZMZUKLNITZIFXFWQAY"
      "VJCVMDTRSHORGNSTKX9Z9DLWNHZ"
      "SMNOYTU9AUCGYBVIITEPEKIXBCO"
      "FCMQPBGXYJKSHPXNUKFTXIJVYRF"
      "ILAVXEWTUICZCYYPCEHNTK9SLGV"
      "L9RLAMYTAEPONCBHDXSEQZOXO9X"
      "CFUCPPMKEBR9IEJGQOPPILHFXHM"
      "IULJYXZJASQEGCQDVYFOM9ETXAG"
      "VMSCHHQLFPATWOSMZIDL9AHMSDC"
      "E9UENACG9OVFAEIPPQYBCLXDMXX"
      "A9UBJFQQBCYKETPNKHNOUKCSSYL"
      "WZDLKUARXNVKKKHNRBVSTVKQCZL"
      "9RY9BDTDTPUTFUBGRMSTOTXLWUH"
      "DMSGYRDSZLIPGQXIDMNCNBOAOI9"
      "WFUCXSRLJFIVTIPIAZUK9EDUJJ9"
      "B9YCJEZQQELLHVCWDNRH9FUXDGZ"
      "RGOVXGOKORTCQQA9JXNROLETYCN"
      "LRMBGXBL9DQKMOAZCBJGWLNJLGR"
      "STYBKLGFVRUF9QOPZVQFGMDJA9T"
      "BVGFJDBAHEVOLW9GNU9NICLCQJB"
      "OAJBAHHBZJGOFUCQMBGYQLCWNKS"
      "ZPPBQMSJTJLM9GXOZHTNDLGIRCS"
      "IJAZTENQVQDHFSOQM9WVNWQQJNO"
      "PZMEISSCLOADMRNWALBBSLSWNCT"
      "OSNHNLWZBVCFIOGFPCPRKQSRGKF"
      "XGTWUSCPZSKQNLQJGKDLOXSBJME"
      "HQPDZGSENUKWAHRNONDTBLHNAKG"
      "LOMCFYRCGMDOVANPFHMQRFCZIQH"
      "CGVORJJNYMTORDKPJPLA9LWAKAW"
      "XLIFEVLKHRKCDG9QPQCPGVKIVBE"
      "NQJTJGZKFTNZHIMQISVBNLHAYSS"
      "VJKTIELGTETKPVRQXNAPWOBGQGF"
      "RMMK9UQDWJHSQMYQQTCBMVQKUVG"
      "JEAGTEQDN9TCRRAZHDPSPIYVNKP"
      "GJSJZASZQBM9WXEDWGAOQPPZFLA"
      "MZLEZGXPYSOJRWL9ZH9NOJTUKXN"
      "TCRRDO9GKULXBAVDRIZBOKJYVJU"
      "SHIX9F9O9ACYCAHUKBIEPVZWVJA"
      "JGSDQNZNWLIWVSKFJUMOYDMVUFL"
      "UXT9CEQEVRFBJVPCTJQCORM9JHL"
      "YFSMUVMFDXZFNCUFZZIKREIUIHU"
      "SHRPPOUKGFKWX9COXBAZMQBBFRF"
      "IBGEAVKBWKNTBMLPHLOUYOXPIQI"
      "ZQWGOVUWQABTJT9ZZPNBABQFYRC"
      "QLXDHDEX9PULVTCQLWPTJLRSVZQ"
      "EEYVBVY9KCNEZXQLEGADSTJBYOX"
      "EVGVTUFKNCNWMEDKDUMTKCMRPGK"
      "DCCBDHDVVSMPOPUBZOMZTXJSQNV"
      "VGXNPPBVSBL9WWXWQNMHRMQFEQY"
      "KWNCSW9URI9FYPT9UZMAFMMGUKF"
      "YTWPCQKVJ9DIHRJFMXRZUGI9TMT"
      "FUQHGXNBITDSORZORQIAMKY9VRY"
      "KLEHNRNFSEFBHF9KXIQAEZEJNQO"
      "ENJVMWLMHI9GNZPXYUIFAJIVCLA"
      "GKUZIKTJKGNQVTXJORWIQDHUPBB"
      "PPYOUPFAABBVMMYATXERQHPECDV"
      "YGWDGXFJKOMOBXKRZD9MCQ9LGDG"
      "GGMYGUAFGMQTUHZOAPLKPNPCIKU"
      "NEMQIZOCM9COAOMZSJ9GVWZBZYX"
      "MCNALENZ9PRYMHENPWGKX9ULUIG"
      "JUJRKFJPBTTHCRZQKEAHT9DC9GS"
      "WQEGDTZFHACZMLFYDVOWZADBNME"
      "M9XXEOMHCNJMDSUAJRQTBUWKJF9"
      "RZHK9ACGUNI9URFIHLXBXCEODON"
      "PXBSCWP9WNAEYNALKQHGULUQGAF"
      "L9LB9NBLLCACLQFGQMXRHGBTMI9"
      "YKAJKVELRWWKJAPKMSYMJTDYMZ9"
      "PJEEYIRXRMMFLRSFSHIXUL9NEJA"
      "BLRUGHJFL9RASMSKOI9VCFRZ9GW"
      "TMODUUESIJBHWWHZYCLDENBFSJQ"
      "PIOYC9MBGOOXSWEMLVU9L9WJXKZ"
      "KVDBDMFSVHHISSSNILUMWULMVME"
      "SQUIHDGBDXROXGH9MTNFSLWJZRA"
      "POKKRGXAAQBFPYPAAXLSTMNSNDT"
      "TJQSDQORNJS9BBGQ9KQJZYPAQ9J"
      "YQZJ9B9KQDAXUACZWRUNGMBOQLQ"
      "ZUHFNCKVQGORRZGAHES9PWJUKZW"
      "UJSBMNZFILBNBQQKLXITCTQDDBV"
      "9UDAOQOUPWMXTXWFWVMCXIXLRMR"
      "WMAYYQJPCEAAOFEOGZQMEDAGYGC"
      "TKUJBS9AGEXJAFHWWDZRYEN9DN9"
      "HVCMLFURISLYSWKXHJKXMHUWZXU"
      "QARMYPGKRKQMHVR9JEYXJRPNZIN"
      "YNCGZHHUNHBAIJHLYZIZGGIDFWV"
      "NXZQADLEDJFTIUTQWCQSX9QNGUZ"
      "XGXJYUUTFSZPQKXBA9DFRQRLTLU"
      "JENKESDGTZRGRSLTNYTITXRXRGV"
      "LWBTEWPJXZYLGHLQBAVYVOSABIV"
      "TQYQM9FIQKCBRRUEMVVTMERLWOK";
  char *hash_str =
      "TIXEPIEYMGURTQ9ABVYVQSWMNGC"
      "VQFASMFAEQWUZCLIWLCDIGYVXOE"
      "JBBEMZOIHAYSUQMEFOGZBXUMHQW";

  bool_t r = 1;
  MAM2_TRITS_DEF(data, 3 * 2673);
  MAM2_TRITS_DEF(hash, MAM2_SPONGE_HASH_SIZE);
  MAM2_TRITS_DEF(h, MAM2_SPONGE_HASH_SIZE);
  curl_sponge c[1];
  curl_sponge_init(c);

  trytes_to_trits(data_str, data.p, strlen(data_str));
  trytes_to_trits(hash_str, hash.p, strlen(hash_str));
  curl_init(c);
  curl_absorb(c, data);
  curl_squeeze(c, h);
  // TEST_ASSERT(0 == trits_cmp_grlex(h, hash));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(curl_test);

  return UNITY_END();
}
