/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_MODEL_TESTS_DEFS_H__
#define __COMMON_MODEL_TESTS_DEFS_H__

#include "common/model/transaction.h"

static int64_t const TEST_VALUE = 123456;
static uint64_t const TEST_TIMESTAMP = 1477037811737;
static int64_t const TEST_CURRENT_INDEX = 2;
static int64_t const TEST_LAST_INDEX = 5;
static uint64_t const TEST_ATTACHMENT_TIMESTAMP = 1477037821737;
static uint64_t const TEST_ATTACHMENT_TIMESTAMP_LOWER = 1477037831737;
static uint64_t const TEST_ATTACHMENT_TIMESTAMP_UPPER = 1477037841737;
static uint64_t const TEST_SNAPSHOT_INDEX = 2345;
static bool const TEST_SOLID = true;
static uint64_t const TEST_ARRIVAL_TIMESTAMP = 1477037812737;

static tryte_t const* const TEST_DATA =
    (tryte_t*)"LQZYGHAQLJLENO9IBSFOFIYHIBKOHEWVAEHKYOED9WBERCCLGGLOJVIZSIUUXGJ9WONIGBXKTVAWUXNHW";

static tryte_t const* const TEST_SEED =
    (tryte_t*)"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
static tryte_t const* const TEST_TAG = (tryte_t*)"ZTDIDNQDJZGUQKOWJ9JZRCKOVGP";
static tryte_t const* const TEST_OBSOLETE_TAG = (tryte_t*)"ABCCCQDZZZGUQKOWJ9JZRCKOVGP";
static tryte_t const* const TEST_TAG_NULL = (tryte_t*)"999999999999999999999999999";
static tryte_t const* const TEST_OBSOLETE_TAG_1 = (tryte_t*)"TB9999999999999999999999999";
static tryte_t const* const TEST_ADDRESS_0 =
    (tryte_t*)"XUERGHWTYRTFUYKFKXURKHMFEVLOIFTTCNTXOGLDPCZ9CJLKHROOPGNAQYFJEPGK9OKUQROUECBAVNXRX";
static tryte_t const* const TEST_ADDRESS_1 =
    (tryte_t*)"RJBYLCIOUKWJVCUKZQZCPIKNBUOGRGVXHRTTE9ZFSCGTFRKELMJBDDAKEYYCLHLJDNSHQ9RTIUIDLMUOB";
static tryte_t const* const TEST_HASH =
    (tryte_t*)"DLQUUDSOIGZLEYYOGXICWWCIBHRPLOGRSLAQNTIJXOBTCEXAUNIKKJICJKTKZ9SLOWSTDBZJETADPFLFD";
static tryte_t const* const TEST_BUNDLE =
    (tryte_t*)"AAAAACROIGZLEYYOGXICWWCIBHRPLOGRSLAQNTIJXOBTCEXAUNIKKJICJKTKZ9SLOWSTDBZJETADPFLFD";

static tryte_t const* const TEST_TRUNK =
    (tryte_t*)"JVMTDGDPDFYHMZPMWEKKANBQSLSDTIIHAYQUMZOKHXXXGJHJDQPOMDOMNRDKYCZRUFZROZDADTHZC9999";
static tryte_t const* const TEST_BRANCH =
    (tryte_t*)"TKGDZ9GEI9CPNQGHEATIISAKYPPPSXVCXBSR9EIWCTHHSSEQCD9YLDPEXYERCNJVASRGWMAVKFQTC9999";
static tryte_t const* const TEST_NONCE = (tryte_t*)"BBBBDNQDJZGUQKOWJ9JZRCKAAAA";
static tryte_t const* const TEST_DATA_ADDRESS =
    (tryte_t*)"9XV9RJGFJJZWITDPKSQXRTHCKJAIZZY9BYLBEQUXUNCLITRQDR9CCD99AANMXYEKD9GLJGVB9HIAGRIBQ";
static tryte_t const* const TEST_SIG_1 = (tryte_t *)
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999";

static tryte_t const* const TEST_SIG_2 = (tryte_t *)
    "ELMOHGRBDLMXEXXVYJTXMBNMCPAZEQ9XHQSD9ZQTIH99CEKIBZVZDXTSVFCQDBZITQQKLCGVAZ9"
    "WG9ORDSD9YYILVJOAFFDECSIFXAWGXQUCWJB9XXZGVPMMJNVLMB9EQQYMHTKFYWBC9X9QDVZVRN"
    "OWKWNXIOJFWYHCFYFVJNVVRBOTZWGLFLVYUDPMDEUSWHOMKJKGGALXDDYVHUN9MLVXCJAAZNOGO"
    "DIDDKLAXZTWZ9JNUMABZOFRBSYTOWBHLI9HVXMLLNZL9VOF9GVLT9TXGVOBONCX9OBGJUXWYLLV"
    "CTVTLR9REHLJDSXZYXTLVZ9WE9ZAPZPXQEZTOPHWPKYERSEDRRYHHIURFUOHBDFXTCLK9FTL9OU"
    "NAMPKPGTMYYH9XNSWUSJEQGZNVXSBCIUUFXUFGPOO9YSMLJC99XY9QPKCUJGWAQIHCUCMFACUWW"
    "AWYAFXTZDFRWPEUMJPYTVILKKNYCIPBBMDR9XNKHDFMAFCPPLDWTNKHEWEFEYJT9PTWYWTPQYBZ"
    "IXYAOXNDKQXHILGJGBQUNMCDCHYMBBCSXNDDZHFPGDCRKZVV99KCDKGOXBRIBVRYTARADJUWXLE"
    "YXZ9UZUKGDVBGTWEFCRHGBOTPYRKUHWUN9D9RHUUQTTIRQP9PHUTERZUTI9J9OBPGGYTEBCRDLV"
    "CBMUISLML9TEVJHZZBHJXJKWBGTXSFR9TO9ZMASSNTTFRGTITEZBK9BNMLVZRNHUFPPENZZAQNQ"
    "AMC9NREHKXGDGFQNOZBFNIZC9BXRSMSSN9VERZYLCBHGVHBPOXIHRIBUTBJDDAFZRETWLHKFRUG"
    "ROXFCYBATBBUVSYQJTA9IFCLCKCQEUXIOBVRIIMHSVZZTSNHQPFXFVJBOVMX9CTURAWQQGYEY9H"
    "BZOKDTSR9CSQNR9HZBPNYEPPRROSQCYWPXWLTCZGSXSAKZTXQZCHOBMVDFTWZQJPFGOYODNWIIJ"
    "JXHHVXDIBBPRHBHDGVLINVNBOBOQJTPTHRCGQMGFZNPXTJXGDPROPRTLUOVC9BVYOJNDHOVAOWR"
    "ISCZGJOJNTAGBRIL9JTUT9LKNROTLEBDGZCIGCJADHOJFWZIU9QWQFLVQVXJUGROXLFMWNUR9GU"
    "QZZKWTNJDGFXXERQYYFVSFYQSNHE9NDCXNPECXUZOWMOONOQQVRLZSRUB9AXHZSFWWTPRSYV9IC"
    "ESWTQQZDRI9K9ZCBAQDRZD9AZERKCCKGSHUDNODKPUUYESPKICJHS9WGMOTOVMPWSCYGUIQTJVS"
    "WXRPUMZHMWWLKCVHLSJ9CFPK9KQTXDKFGEQLLKTFGKV9VTTMMNAHBMRWC9FTXOUVWWXSEPCCFQE"
    "FOJBDGVIXGKGKZPGIBOZWD9HKMWU9JOGUVGFAQDRTMLHTQJUT9KAJPETOYHMQARTWLZS99OMVW9"
    "AFCJB9ELSDJRNCPCKOUDPREDNUXVV9FMDBNFK9QAYNFLEIIAYWLEIPGO9JLAHCUBMYYFYDLYVSW"
    "POAFBYEFEJBLCYOAFVRSH9FBTEHCEPKVV9IYOVBBJUCJBL9GMUXQTPUJWXDXHNNWDOURSYHMGYI"
    "ZACJWZLMXBWMNZQNCQEEHGSDMZKXCDV9VTXXLVTABJEBC9OTIGPYFYUZNPRXWWHHHZSOGYGIQTI"
    "CHIKXPXBBJDSBGTSQFBIRHMSFMYOZGBXDWFDHLDETSNJLY9WQADIFVMXWYTBVDAARYPNKFNNDMS"
    "W9DISICWVBJCAYOQQEQAUGRRFMNNIW9QAAGNLJPBXJCSOUTCKJUBSXYMYYPJWAIECBSA9WMI9KP"
    "CDWZLOXIGH9LAKVGZDNGMD9RLPWJM9NTIZIIAGYDSOQSGCUSLWYVLJGHBWJGR9WLHAWZNFAWQSP"
    "9CPFNCEEZKJUEEMMIWDWGC9TQVUNYLXEDZOZL9CZFSXPGQUNNRQTQNYXVMPBHVOPRPFWBFDMPZA"
    "AZWMDLDBLTODXBZBSRRCMCEBXIOYGENPNALGNHDQI9GNKOMQXLKT9OYRRKGVESBLE9XSUMU9BAZ"
    "LLZENWSLIBKJOCQHQPLRWU9HCUXILKFLRAXWMJBPQNQNZIRWUB9UEE9PUAAIEYYBOMIHU9TTASU"
    "EMLGZDW9YAUFEWQJQBRW9RWTFFORNRBLCVGXIIUWBPZQG9GTKCFWRCSVAELPDDMNQC9ATKDEKPH"
    "XOPWATWCUYYA";
static tryte_t const* const TEST_SIG_3 = (tryte_t *)
    "9EQMROMJJBRUSQMPAJWOUGDTKJXXAYATKUZRUDSGVTPRUAI9YYMYAFNDDFSDLFFXANRFUWMBW9"
    "RSFP9LAGTFQTG9LZIZPVN9MQAXQWGKAKP9QCP9GSSYXMNSWVRL9HYXTFLHXYONVCYXHMCFHXNI"
    "QSRYTCVUQ9OVLCWOARZAUZXLDMYUHR9HXUXBQSETZEGKYBBYGTDPTCNTBLOIZX9SAWTUQWTLLH"
    "OMBOPMFFVGHTFFXZCRRJZUFCKXBLWEZGJTMFSVZDNAYAZAKHTHIYANETHXJPTXYBUIKTMEODNL"
    "JKLTJRSUVVCWRZCWSEBSNNNVWMDCHQRVQK9XHWWNLQJRQHWTCZVZQXPVPYJIUCFVLDKSLD9UA9"
    "KLMSBISATXYFPIKKKMSZYEDPHRQWYXDZDICDYUMSYUKPVINGAXY9BVDSEKY9NWAXADYHWNQHPW"
    "YXGKPUZRESUJSTZ9AYZWFIFWQRTCXPODGDGWUMKRLBBJRWKWROOVECPXHXPTYKPCDDXSEUIBVC"
    "RPOABQYLMWEJQEUUKQSZEYUOVKRPMZYPIMPDNETEBJAPJIBGCELYJKKE9PBTTCQPQVWAIFXQOA"
    "SQCTNFKKGDEMILAUQWGYJYRANV9DMEIEAKSTDQTDIZKHJTEFDMEMX9TXFVVNDJNYEDBSNLZRIB"
    "VKWGQPGSXMEGJMLRIH9BLLYLEFIROLFFKS9SIPBALUPTTLZSXLIARSABVZAJJTCVYONRBDSJFE"
    "ZIWQUCTNWDRHJVEQAOHBKCXI9CUZLMSFYUWHXPIIOHPCDOGBG9RQLKMWBLWFOKBHXMWIKZPEBR"
    "JMFI9EAKW9OYDL9OMW9WYUHCQOTTDZBAENNFBTFS9ZI9GNGHFZVVKQSGIJOGQBXSLUKLMXLWGI"
    "XQXFMYKJZKRCCKBPWOQPXOVCEJEVFGCOD9FEYUYGMIYSEWJUPVFKBDKGRVIS9OFPUVIOXJWZVD"
    "DWZPEXHYCCHPDFNKFVHTSYNZJR9RWFWKWGAATZJLIIZM9NBACTDSHQFNPBWRTHKRJ9GHYTJILK"
    "9ULTEIIEZQCKOAGEYJ9RAKJOICVKBRKEXCGOWON9FQZDHIYEGJEYHTFNJIRFGVSILTNERRKQ9I"
    "RJCLBYPSBESEBNVNVGOCSBP9GFZTFEHIDIDOU9RMFWUU9XFRKXYLKWZFEDWYRIATFIBTTKGWFX"
    "KLIFCASUAEJUXDMATAMPLDIBQV9BYUYSYLZKTLQOBA9QUPSFNVCHTBCAASXSYGGIMOGXXAEPGK"
    "YIKSYGJNIGHJNIHAUIIDK9FIBJOOGTSPNCGYDDYBS9AHIQMBXNAPDQEMYQJZMQLWAYCECIJYH9"
    "GLVJUXM9QYKTLWBTYJKJVEOZSAVHVJZN9HLCCPXVNJMTZVXGNVB9UGQVMYJBWBWEETMXYOWVFJ"
    "VFUSDBWXNFEOWRINGJRVS9BEJXAGMVMTBZDFGCPDVQ999ITDRHWWKWUSRMQWUFOJBPBANVJGPB"
    "QUZICQLDMLYKZTRPJ9VMZREJUPMCZWPB9NWXFAZQVKWVJBDJ9GYW9CPKEGZOXLJFAJYDZBCWC9"
    "UPRRHRKAQZBCROFEMCBGPCMBDWVEVJXERCZRRDEKJOLWKEWCLXWJ9YGKUUZ9MVVZUXCPFIJDDF"
    "SPWLSWKIDWD9ARPQIJKHAAFRLAPUWASLGKEBHYPMWEPNAIDOOTZAOLXXMPLKWULWZVMHDMKYCT"
    "GPXMOVFMGPSWBRPPPL9ATXCPTEXVKGMLXQTFHMUOVBVTOLURCSTRFNIDRIGXYBBPCUPOWNPBOE"
    "ILMCWXEAIKVPNAOZCHJELHALXOVEIVXPDIFGUXBJKFNUOBZMJSXAJHSCAQTMFCKMQJKHJQAIGB"
    "PZU9PGCQSRJUDCE9POPQYUCGHE9EASO9GWHRSUEWHQDJTTLHTWXZTOVQNMUSYSEQAHGSUNTVTB"
    "NZGHUJUS9EAVIRUCKFVYHYJQUIRG9XIMVIEHQAKLLPSYAWSEYASMJDAFNEA9VNAUWGCIJZNYIH"
    "WGPBFIGBKE9MVOGFHRMYDCHMANXRZMWINZZURC9HOCAEURFTYYWNQZZVFFNASPJKKD9TCYXRLR"
    "WHNCUGAQRIGOWZBVYZ9TREWGIKGUEYAQAXKSJOVUPHINLJZIXIIPYNQNAWGWKSJTISLGDQGQYQ"
    "TUHIYQTZETGFWFFAKRBMYTYUQRPVZWOPUJWFQFERX";
static tryte_t const* const TEST_MSG = (tryte_t *)
    "XQRAJKUGVHTUPGHTUVBDLIYBMIUWVSPWZB9JOERRJPAILRCXEOD9CYIS9N9PYBMKGXIURKDOFA"
    "CDIPFTRUROCZYJOOSJGALKKSOUAVCBGUBIQBFBANVUGKJNBYGTFZOHHBG9WFXJYUDMRSKUHOWA"
    "HEKODFBGTHUGFBVZYSKIVUQTRWLFEQYUNGVSVXYCUMGEYDAXEEMBETGPYPITUFKCXFYXO9NXVS"
    "JFKHOFILVCJQLXW9JSBNCEVWSHAVCVBBBLXWISOWYITIECXFPUJEFMKECADBICSLAAVNDAGXMZ"
    "UKYRCXEMQCRWTMVHVFYYPIDLVQCCPXBDBLLDKOQAKBEPVBUGKEMFUIXWIDLPD9ZZSSXKKWSFBJ"
    "WQ9JSSYULLG9M9XUK9SSIBNLRI99AUZUYWBSBJ9DPNRLZTAOQAEIDTKCFEQTPNLBGWZQFQPXQU"
    "BBSJZVNYABAGAXVF9NLALCKYSMMQRJWAYNDJPSNSMDBJRWKWROOVECPXHXPTYKPCDDXSEUIBVC"
    "RPOABQYLMWEJQEUUKQSZEYUOVKRPMZYPIMPDNETEBJAPJIBGCIUDHEZIQSPCRURXNXTBHPVYR9"
    "LIEFRVMZNB9DDBTCNQOTNEQNJBQTNFCCDDQZHQGURUBBHVGMARLIDGD9WVUTYFXNTVLAENF9DQ"
    "BUKITNHWFKOYDB9RAOZTSNROGHXUILWKWS9HEDGNUEGIWRRPFYHPMWUCSHXUOU9AKAAZPWEDPG"
    "HDGIEKIFWFYYLCPMGKTKZVMMVGJIHYMMAAFEQJXDZCEXZMNALAKUA9KMJEZBDD9ZMSNPGBEJAM"
    "PAAVHWGCOCJDABHPLXTEJZVFQDTZFNTTJLAUEJJKPKLQCXTNCROMUBBPRBKWBPNFPYFUNMD9VS"
    "GAWNRZWUYOHNVXRQUHYNMLOTISYYUIXKCXNANIFRNDOGNDLOUJRFHWFAJDNTKARDEBDJBQ9QOA"
    "OZNPBVWENXDPPSWLMWUPUFPSYJZVKGRSKLMMI9YYBQEQOZPHWLVFJGLDTNY9VIFLSXXROVRFHO"
    "POYHOEHOBHHZWOKEBSWXGVLPXKH9R9KQUPPAGBIYAY9ALMECXVHGEOHQTKCXWHRDYPVEPUVQBS"
    "SFVFQMJYXNMUHPHXVCVWUOIWRLLQULDGHJOHZTSAKMQDLJKG9JXDONQPZU9FUEDKWYICYCWDXT"
    "ZPIRZZPPUAJAUHDTHHYDG9MUUEQHSLWCWIJKLAJVAUWDCJAEIXEPPUMYUMSYVFXXI9SLXSDFYO"
    "MLNUUUGTBYQHSWXGC9OTNMGLDKTBGMUDVAAXRBGIXLASDKPTEHZTS9AX9RXMSTLFJH9UXILKLQ"
    "QETGPDKVII9HIUCOEZVGOJZFHMJLZPGKGNTFWGWZBZEVYIQGKMORCXUQQJZJYNAHSAVBEGTBWE"
    "VWMKEMZK99XMJDEDNDAMIXHZUZANCJBVCGLRRBUVLWUXDUQSYFOWQMWEHIOQGDQPYSAXREOEBK"
    "XQRAJKUGVHTUPGHTUVLRCXEOD9CYIS9N9PYBMKGXIURKDOFASDAGGGGMMOGNCCLPWBCZGEIYNV"
    "YKN9JRTUHAERTNUJPKFSTQGXEJBGOYBIVOVSHUXNNJKN9AUCIAJCNB9NDMNLCSHPB9WVMPCHMV"
    "UJVXOOCJM9DTPDKBHNPATGPNETDHZHGHRYCQA9JTPRKGODYYZQQLYJGLMSWPLGOXW9NXQBMDXY"
    "9GPMSLWBYLIH9S9LGLDWDRYAYLYGNBCEAIQCXHTTHSYNCJ9OJDLI9YKETAQLCTCTHTJKJN9UZO"
    "YSBQQCFROHLBQGMYJCSDTISHUYHJZUPAUPEGDWDPVQSKDIENAYXMJDFWYBIXMAZZWZJISFQXYX"
    "HSTKZWWYD9ZYBCID9IWWUPRXAQARZSLXONJSMEPPZCMPGL9FFRKDOAPD9LKCAGOZLECTMLLWLI"
    "KPV9SPWVLZWVFPLSMNYCSMNJGKXNNZROHJPH9OUEWPJPVANDAIWYQHSWDXWYUQNGLSKCGFGRVH"
    "9AFBJRVEDNQGODZYOBUNZINGYBCNIOOGPIBQARWMYGG9YYJENNMTXGLJEQZHLJDSJVCCZMALAE"
    "DNCVRLPDCWBOFDUHZECUQXABVBHOWWLBFBSUZAGYEUUQQWMTZLJCQ9HXIORFXKUTIGRFUZDXPF"
    "LMRASRIVQABGUZBQGUPCVPQPILYQEKMQVGFCGXKAD";

static const char TRYTES[] = {
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', 'A', '9', 'R', 'G', 'R', 'K', 'V', 'G', 'W', 'M', 'W', 'M', 'K', 'O', 'L', 'V', 'M', 'D', 'F', 'W', 'J',
    'U', 'H', 'N', 'U', 'N', 'Y', 'W', 'Z', 'T', 'J', 'A', 'D', 'G', 'G', 'P', 'Z', 'G', 'X', 'N', 'L', 'E', 'R', 'L',
    'X', 'Y', 'W', 'J', 'E', '9', 'W', 'Q', 'H', 'W', 'W', 'B', 'M', 'C', 'P', 'Z', 'M', 'V', 'V', 'M', 'J', 'U', 'M',
    'W', 'W', 'B', 'L', 'Z', 'L', 'N', 'M', 'L', 'D', 'C', 'G', 'D', 'J', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', 'Y',
    'G', 'Y', 'Q', 'I', 'V', 'D', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', 'T', 'X', 'E', 'F', 'L', 'K', 'N', 'P', 'J', 'R', 'B', 'Y', 'Z', 'P', 'O', 'R', 'H', 'Z', 'U', '9',
    'C', 'E', 'M', 'F', 'I', 'F', 'V', 'V', 'Q', 'B', 'U', 'S', 'T', 'D', 'G', 'S', 'J', 'C', 'Z', 'M', 'B', 'T', 'Z',
    'C', 'D', 'T', 'T', 'J', 'V', 'U', 'F', 'P', 'T', 'C', 'C', 'V', 'H', 'H', 'O', 'R', 'P', 'M', 'G', 'C', 'U', 'R',
    'K', 'T', 'H', '9', 'V', 'G', 'J', 'I', 'X', 'U', 'Q', 'J', 'V', 'H', 'K', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9', '9',
    '9', '9', '9', '9', '9'};

#endif  // __COMMON_MODEL_TESTS_DEFS_H__
