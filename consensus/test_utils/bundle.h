/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_TEST_UTILS_BUNDLE_H__
#define __CONSENSUS_TEST_UTILS_BUNDLE_H__

#include "common/errors.h"
#include "common/model/bundle.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

const char* BUNDLE_OF_4_TX_VALUE =
    "XPXXGCKFSPJB9NKPQEFVEIRCTLEOZJRCQHJNLJRHXKAAWIFUBUVQZKBNFVYPLSLERDZOYSEJCG"
    "IRH9NLB";

const char* BUNDLE_OF_4_TRUNK_TRANSACTION =
    "9RVERIBISNTHFFNZBBXYPSFCFQKKSHVNQLTXWWLVDBVQJFRDYZVRUKIDGILWNU9JQPKVOHSLMA"
    "PVLQCYSVYQJFIIEVUXWZPUVRWZUGAZGKNQWIJVJTHHWFWMSWAPWPANCDAPPDELETUGUNIF9NZT"
    "VIOTLWPWZVMAH9YTBAHCGHIVICBQZWOQCIT9QKIHHIHBXHHDVNUGUIECVHAULJZFBZZTQKFQXN"
    "ISKCFLQKZQDNNKNDGAVWCMJFZZBVSVDCVWKVA9UDIHTGNDWNXPPZQIZJW9SDHYTTWHHU9GRQKL"
    "MMDXENMOMAKGBRGBMMLLZRHICDKZGQNEICAJ9W9MVYFQZEQVSSQKR9CBKRQEZU9UHHOABSIGKS"
    "JG9SMRTDWHZMUQXPXPRD9N9XXBYHHQVTDQKCAWEWPVANPAVYUMGLVWSUBKBKOXRPVOCOAHAIA9"
    "AHFAG9HRQVFQNVHEXAGVCPAQEIDWGOEVKRM9VDFFKAGBUKEIPFT9GPXHOOALEXWG9HEGCYRFRH"
    "BRJVBRQQGHPUBLE9QYHHXVAYNDOEKTKKHVDWN9YQCTYMZPLBAZLQLGRBETLBFZKDZOXEMUDWAN"
    "ZBILFQ9JGQL9GVUKACEWAAGHQPUZWSBAAMMXJVHKZSWQIXZWPRZPHUSBYIQFYKBEY9UBMBBRDY"
    "RMDXFFZKERHJMZBUHWZLYYREOQXFYAXVZEFCPSNTXZZWZNQXBYGQGQYLDPJBILPKSR9FLTHLHK"
    "TFAVYABMCGFXWDBXZVQXRL9UQGRQKKX9YMXCERLGFNZXMJASLPVULSUBCAKXU9DVGPCMAHKIMC"
    "SHGRDRYUXWFBGZG9ULYDAFEUABX9PHTORUWIDVEAGAZNKLQHTFUHPHDGYLJIPGABYZPXPJRVWQ"
    "DJLOBRSAFBINV9UBGVXJRXBPMDTQFPT9HYMAEDXEXLYEJJSHIQHLJHNI9NMXKPVGZCFKCOHGK9"
    "ILYWJYCJREENBOWEKZAEIVGIZXRHHUOCURHKEHVMZAPMMOAGXBVHDRCAP9JHRJJINELSECIZQI"
    "JOCESADQDXWBIOVUNWSHDWLKXRCAHZW9UIGYBAR9YHHVZNM9DYM9UWSEI9EPUSIZKOUSPADYIO"
    "X9CUKCULOUJUQFXNHIGBGMBHXC9VZPRVDTJASMGCALVXUGQCCAKBUIJJMVAQE9MTUHZETFJGTD"
    "JLEIXA9ILHCUIEBLNDNKLVBWGJDCFXFYUZJJ9TUBJXUDSJQDBTLLGXZGYUKDBQGCLQFYAPBZGE"
    "CTDAGAKJFRVDKXM9OFUFFBBOMUPUFWOSTVSKASCF9VWDUBTWNGBTTXIZXEVBTAS9LTOETNGNMU"
    "COXKPNVLDK9F9OYMN9OJZQHQHT9JGHBRYDINJKKPTDXJWLH9MNLJTLS9FSQIMIXYYIDQGKBQHY"
    "OQGZUWUHUWXG9C9LSYDTWRFFHDZM9DVYSNLA9AMNKOL9DAJPCFBFHZYZMDWSIFVGDLHETRINJJ"
    "9EVWMZRLCMFJYYSSJNXXMPB9ETLAWGWAZFGWUQVPOXEMKDIHYJXNXHT9N9ZJWTIAGAWPWD9MHQ"
    "GIWCUZXDCYRODRTYOVHTJK9UBQVCQPWWYJMHNPKLDYWKUHHYUTMKYRTWELLBOMVEQXYRCNPFZJ"
    "NV9XS9MSAEDXXMSEDJCAKJJ9KTBZVZFAPELSOJAVJUAVFCLOJBJGLXHZNFYUNLKWKVPIDIPXUX"
    "BVGQZSEYFIAZOSLEISHDR9DRCHLUUWXQIVBBXLLOSABEVRSNGMYSIWMOLYBRJGKYJDWNDBHLRW"
    "WORXGGQVPYYLUECMI9PZSFWOBFSZFYIRWFSHBGYZEFMDFGNQOZUEVWKIFXVBIISPSGAJGEHZRY"
    "XBNCETANAPDTAWPHBAZXZSFOGGXPGTIZIJRXNMFYATDBZ9RGFBFNCYDUFHI9RX9BFHZKJHVL9Q"
    "SITZWVISMQGJHWQGYHLMSOLMTGMTKZNUIMYJVHZ9PSNZDDOKNAXVYNGSIULJRJGEKKOJOLEGOD"
    "MPOEQOWD9FDCUISETBAOGFPYPDUIHGCMBPQDHOYRCEANSCNMFJJEIWTSPQSXNYJTHANJHJOAEE"
    "MRPXNYQJVECEQ9S9IGOFVLFRVKGUPPDAQT9CPJZILZJK9YAPXMNMEPXTORLAJIFPNNVZSTLAOL"
    "Y9AQTDKAHGSNAZKWIDLWBXZPJTXWVBFXCFWXTXRTZKPWCHICGJZXKE9GSUDXZYUAPLHAKAHYHD"
    "XNPHENTERYMMBQOPSQIDENXKLKCEYCPVTZQLEEJVYJZV9BWU99999999999999999999999999"
    "9EKVLA9999999999999999999999AJVWCZD99999999999A99999999SJLBISWQTONKVZPPDHL"
    "SPTWDETCNKGW9KNZ9NBDYDDAQSUBLZRWMFSUVECEARDHOJVWLTJ9VFYKVTQIUYLINC9GLMVBPC"
    "ZJHEXPQIBNNDBIROSBHXVBVOOPUWUDDAIIWLKPDKJMTEAQOSSZQQIDAHZXTEP9GAA9999JILXY"
    "HMATKDWMWYSMUAHZCSATXFJE9MOBTPLAQTZEDAOJQVB9ADQWIYRFLS9NB9MHC9ZY9KYBBMYA99"
    "99999999999999999999999999999999999999999999999999999999WURHSEBTKCN9ROCKER"
    "SLKTPOGSG";

const char* TX_1_OF_4_VALUE_BUNDLE_TRYTES =
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
    "99999999999999999999999999999999999999999BFVRVBTBVCMKXCLDUZDFWQWYCGHPJATRY"
    "FDLIPVPFLFYTWGRUUOIWXXHOEKBLZGPFBICLNGEMEUPKCSCWOEXNOXD9999999999999999999"
    "9IYINITY99999999999999999999KGVWCZD99999999999C99999999XPXXGCKFSPJB9NKPQEF"
    "VEIRCTLEOZJRCQHJNLJRHXKAAWIFUBUVQZKBNFVYPLSLERDZOYSEJCGIRH9NLBLPFADGOKKAJ9"
    "GVEGJMJ9JPDTKLHTGKZEHHPZP9AOIWWU9VDTPDWGDKTSMDBMXYFMFHVPGUJPQPRKZ9999EMGRZ"
    "G9SXUO9LIBCHKOOSCTZFNIHRICNUSRXADAQWKCGWKGFXNLXDLKQMIEDLAWHTSXDOQZPDIFPZ99"
    "99TRINITY99999999999999999999HKQOKWYLE999999999MMMMMMMMMCZUGNKPECUPYMONRJW"
    "P9UEXXB9W";

const char* TX_2_OF_4_VALUE_BUNDLE_TRYTES =
    "QJTWZFTJRSI9OPSDVKHEULMAAAJUL9LAYBIDVDXB9RQEQULSGWLSYIVDSSRLWFWXGDOCMVBDZL"
    "ZWIZKKBAUHZQXXPJEAHPDQEZG9GAJNIRACNIVXBAXPVUCPKIIKWGUYBWSRMBWBMHOMNYRJJDYT"
    "VIGGTJDJCKUVU9BEPOCGDXBLEJWBFGIDUHQRWRZEWWWMGUQUCWSQCXEJUYZ9DODUETSVNWRKHV"
    "XWNZZIYZGCDQDIXCWEHVZEZYOCDZYQDNBVEILTQHROCWPNVDTDRXNWBCFQXXSLMAJKUJDGJJSN"
    "GAZP9ZKMBL9TLGFUKKSCD9OKWSDWUAURH9CLTK9AKXJDWXXINNDGXNSPYGZDZQ999K9ZVJRRVD"
    "ALSFCZVNAWYTZVVCLXLSXWTRLPWNHYRBCSDPVYCMUREXSJYKZUAEXDFVDORZRNFDIBLJBAVHCO"
    "AZGCVSUONEOWFDUOTIML9IFRFKOHLZTZASP9NRIGUDLFBUTFQVTVGUPAPIKE9RH9V9MSTSQNRD"
    "SPLBZIHVNDIEUJSENHPQGOTAZJCDVEFLEACDNPLN9KLL9CBUDGWAJPMJVQRSUKNEVQWKZCWOPA"
    "O9FMOHRWABKQBDMUWQIWHMCJMTTGKTHYSCXBKQFQKZIMSWNGHWOGFKGDBPNRCJGSJCIXZKIFV9"
    "ZRLYLFWRAYFWXABDWFNEHSTTNCBP9DNTTFRZEIPXIOJZMTTFGNKGEUQBNEOGEDCUTVGFOYHWKG"
    "GBL9VVMUMHPLNVDVBTQQPSUSKJCQNDHKTVGF9GROAUXILLJYHTFEKIZEDFJLFPMYFCXUCYKXNS"
    "RRWAB9IUYGAUY9JUXCAEXOJNKCPWH9XCZDEFEQO9ICPTWSEBUZFPVDAOGWUXUUWFKAYYPEOWHN"
    "DTDJBCDUNUZGAZJVTGVXDY9EQVGUETRIBJWFRMOLVREC9I9GSJGLLDTMZRIYTGJBGTBBB9SNQI"
    "TDLEWLQJSXXIRDPTEJFYCGZSLATDVXBFFFQXK9LNHZPILQBRTBWAEOYCHGBAUKWECVZGMYGGZT"
    "SGIPEEJEKJRQWYRTXPMEDZTBXNFDJPQTOVYRVKNHQUSNZJGIJJZI9GPCKVZAVEFBMO9YWQFKII"
    "INDKOYNCBEFPGRLYLXKUWMDDNSREOWDXCWKOPMUXJYZECDUPSDCMFNPSJVXKEPXBXDFJGDGCXM"
    "LOH9ICAMAKONYVENVKPJY9FJTANNZNBLJNGWXXTZBEBQQUGRIJGJALBXLGFUAVKUIBUCXDTRFJ"
    "XXTZVFI9TXATACCTA9NXOMMGMGINJEIDGM9BEYLYTWLNPLIIPOWWUCLY9HAKUGCXBCKRCQHOPY"
    "RUBYTOUC99HYQCKJ9PPRPJVJDQHO9QVRMU9XMDGQZVYZADXBMKXPO9JAL9VFSHCGXUPKHLUEJD"
    "FYCQWJBNESJVY9UWIXMJBGNRPWVLRJBBGKHBDCBSLEIDZZG9MKEBDUOI9QTKCTARKNPRVOELCW"
    "THBTWZIHSINJBOONGYSSCCRMSLG9VPMPZTZQKDMDSQLEXHBVGETZBUAIKNZMFEWQCIYFQNPNLG"
    "AIVBYBYHYG9APYZCEOSYYWCZGJRBKLJDSDTANSDAKOQFH9ENAJMNEZGGGINP9SVIBXWWWCQSUY"
    "TICLERYQAYWWOMHJL9YDUDIKVVOMKGDVSZFZVIVGGVRSVFYOMOCSXXAGYJZMPLYRTBFSZQGRDZ"
    "HUMYQURJYVBYTBNOUVPUZTSUABWANXNLRHIHQXIDGDTWPLAVMUKQWTHX9XLKSIDIVNJYEDXOUE"
    "ODVRZWJOCWPUQY9BQHZAIEXCVUOFOMIYNMODVVRKIBJOOWPISWIBZ9TQOJXRYL9ZOVVZFAAF9U"
    "ZLUUZEDURBWZYVMKRJFBXUID9RNRPLUEQZIPFNZI9ZUNNHDGFSAYG9SL9NDLYOSACJHJOYGNAP"
    "ZBGXLW9FNAQMUTLSJVSXBIGNQGUJ9TUUXRQESJPHFGBEIFSMYBJBQZBMM9MDOQKIZIQDRAGWPC"
    "IUZGRPSOMYZWKKWTERZFQXEUMFDEFBMIWLCUISWMLRDWT9QWJSSGSGXRQKMKYBWNZPCWLVBWQO"
    "D9TVAPMGQHPGATSBZJWNASRBCVOFAVXSS9MFMYLCEVTKUBKSCHQAAPREAXRVVJ9B9VHQSIRHLF"
    "YSAUOHJ9ZDGNSCFSYYFB9KRWUHMHSAFDQEQCOXOYDJVASWEPDNPDAXXPAKIJTTEL9ONEGMRPUW"
    "WCHEEOTZVUUKHYSXAC9IXQQTPKWMGMABUNVJCMHUHAIFZDWCKGFRI9W9999999999999999999"
    "9TRINITY99999999999999999999KGVWCZD99A99999999C99999999XPXXGCKFSPJB9NKPQEF"
    "VEIRCTLEOZJRCQHJNLJRHXKAAWIFUBUVQZKBNFVYPLSLERDZOYSEJCGIRH9NLBVRMIJL9RB99G"
    "PJHVIXZTBXNFHKBGPLOXYZCTBEHALDBLPXDWEYA9TXLCKXRVLEGNIRNIFDKWEHGF99999EMGRZ"
    "G9SXUO9LIBCHKOOSCTZFNIHRICNUSRXADAQWKCGWKGFXNLXDLKQMIEDLAWHTSXDOQZPDIFPZ99"
    "99TRINITY99999999999999999999HBQOKWYLE999999999MMMMMMMMMCAKRKEYAJIDVMJLTNI"
    "EPZDTBGRS";

const char* TX_3_OF_4_VALUE_BUNDLE_TRYTES =
    "FBOUAYWGHCEKH9GTLXQZYIGXEWVIEY9HPWTBLYOATBSTJZHSSZJXFINOQOEW9LKXSTOCCRJJWS"
    "YRLJTCZOFKZKFPURQKSCNALMAMFNBXGUFGAUUQPWVHZKHIZVYYXMEJPRMTWOXHMWQEETRICMNV"
    "VEHQBTBZDOTSBYOSRBLRGGCGUPV9DOHZRRPDIVWUGTRBSYIVDEXNDJKMVPCLIKTOBRUEWZVLJV"
    "HNYPJZRIRCLGVYBAEXMSXXGYWEHKRQNPHADLUCACLAWFOHBRSXBBKQCCPYVCEUHLUZRKBPVKYX"
    "LWMWZDDBFUJZATJXSNYWXKHERPUWWGLRWGLTWMDNXAL9MACETUHVFSWAFJXLBDAUXWLHNKAA9J"
    "CUSNKZGCHLZMLBLONBUPAINYUPHNCOMYQXWKHPEXIUCDECBQMINFJUATZNEHG9XFOCRGMYSEEH"
    "ZQSZGDONYWXWPQKKAWU9AYIGLZXQHMABSM9FZGNGMCZHYHPWZDTQZYCZWAIUDAORWQUKSPE9DN"
    "TQZGNHXIGXTFLLRWNEJJEU9SRXONRNENRTL9QY9IPMBZSMCTXDEXAALODNYJGZHKNMRL9PHGII"
    "SUZVWALQXCPRGARQXFQNGVQFHJIWHSWVFI9TSOWM9LVXEIMHHCCGBJMYGPVTIRVMOJNUCOLDPX"
    "WFXRMUAXXTURTGXXFWVRSRPNJUNONPHNKTQGUTLDADFGLBJIZBGBKVKFBMSVZCAHXOPUVFYKVE"
    "XMTOUNRNBKRZCFTWFUQVKPANRUSCKQGJEUZEELGWIAQLLCYG9KSPIPEMYYDHEJUHKNVLKWDKJG"
    "RMHAHFZAJQXJXUYHTYQYOPHDIQWUJ9PGZZZSBSMDMOYJODLNZYQSFPZHXJPDIVDDWYPVGMXZLW"
    "L9XKDGTSRVAOFEPINDWFQUAQVCCYIYKELFGRAWXBXXLDCXZCCXDLADFTKKQHRJSCPHMFHUIMRK"
    "MPVHE9TJEAYPZZTWY9UVWHAGXEBOBXFXKKTNLGD9Q9SPFJEUJTTEIFYNAZIHMRINSDOCXIFISI"
    "YGUQZNWPSRDXKQDMZ9XAPMNYNTMPCLADXO99DPQYJRU9KYVFEYZJXAREKBYTPSGHVBTWSUSMMY"
    "MWOSJBDSWQRHADOGPGQMCRIYLNZAJCBUMZMJEZKMCSBLZMH9PQJSXZUDEVXNBSIIFCCAOWL9VZ"
    "MKKJOTKRDNKDXEHQEVQXDHQQWPIGCY9DVGLBGCLHQAGQENNZXFDUAIATDEVFCECFDQUBVSWGFR"
    "NOQUPENPKOPEJIJZMRSYNYQKCTWRNLHKVITJIAMYGUEQFQTXUNMKVFQOQAQGY9IQXUZMXMVZJV"
    "GLQIPZHTZDFYCKMKXXHOFVSERLLYUFZZQSGUXTXQOOTQWMSYHAOBPN9KDEJRMRZKGSCTHL9HEH"
    "CFHVIAFPFTSEQFNCRF9DMZXOAHLLUI9UXXWNZQKTASZIPJKHMWRCVAG9PJRJJIOAWTLGHVMABN"
    "9PVAOHYCUEWOXIOHMYTUTPNQHTATBZ9YCBUFTVIDDFKTNHIRIKSQTVBUCFCODDJFHFNQBSLPVZ"
    "HDKDPABVEWXZMHLTCQOWARYO9P9BJFOWPFGGMNWXSXHROYXJMCSHSEFBJZKDIWLUJCHUJFNGOD"
    "SRGJP9MGOEOZWQBRFGDKBZEZYXYBQYKEZKQHHZUXUI9AFXKVOIQG9XPJHOMKDKXQLRLKRSJPXS"
    "YYLNCZZLFMPDTVFJSSDZMQJHLIFJDYMWPXLNZHRPO9FBY9N9LUNGRUMTTMXXNDKQRWRAZFEHKQ"
    "PR9AJZFQZGVJ9PCYZLNBSRGWHTGJSRFRUAIDIVGYSCLTQZQIUYWXZLZWIT9XTVCVZQJAJQV9EU"
    "MPYYZH9GJVBCYX9RUREQAKNVOXANMTOAYUIAQFLWTEILPSFMW9VXPHHCPSFZXPNSZNADYOEKMT"
    "AYN9UDTHYKMNGTFACKTYXHGYZPFDARQRYDEFWKRPQZOAUBLKQJZQCRHEZ9RKABHMTDZGNMQKQ9"
    "BCBFO9XFEMNOYCUGLKSCIAJFFLCWDSJTWPBCOOAHEEK9YGBCSPKZFJUPVWTDMORVNQGLGCMRZD"
    "OQDIO9ZPFZYQHKFATPLIHURKPDPHCURYAYFDLJFAEDLG9FSMJEPBCWFN9NCHLCKU9JWGPVUXCU"
    "PSSW9JESWXTTUVGXWHEIH99UQUHUCEOGFKGIQRGEAJVASWEPDNPDAXXPAKIJTTEL9ONEGMRPUW"
    "WCHEEOTZVUUKHYSXAC9IXQQTPKWMGMABUNVJCMHUHAIFZDWC99999999999999999999999999"
    "9TRINITY99999999999999999999KGVWCZD99B99999999C99999999XPXXGCKFSPJB9NKPQEF"
    "VEIRCTLEOZJRCQHJNLJRHXKAAWIFUBUVQZKBNFVYPLSLERDZOYSEJCGIRH9NLBQH9LDBOHOZQ9"
    "IYJLMFWKMYVEWVNAVARPULUD9QKAVGNGYXGKJ9DKCQPCUSPFNUJ9GGKHIH9FZRFGZ9999EMGRZ"
    "G9SXUO9LIBCHKOOSCTZFNIHRICNUSRXADAQWKCGWKGFXNLXDLKQMIEDLAWHTSXDOQZPDIFPZ99"
    "99TRINITY99999999999999999999ZHMNKWYLE999999999MMMMMMMMMM9ZMVCIS9NNZGRI9FH"
    "YKSSWPDUK";

const char* TX_4_OF_4_VALUE_BUNDLE_TRYTES =
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
    "99999999999999999999999999999999999999999OQHOJITNNFIBFEYRVIDVQZYGYBNKLUKSV"
    "AEOVBJQDZIDRP9ONQJCIBELYD9I9CDLVU9XCFRMTDFJUVUDCAOXVDC99999999999999999999"
    "9TRINITY99999999999999999999KGVWCZD99C99999999C99999999XPXXGCKFSPJB9NKPQEF"
    "VEIRCTLEOZJRCQHJNLJRHXKAAWIFUBUVQZKBNFVYPLSLERDZOYSEJCGIRH9NLBEMGRZG9SXUO9"
    "LIBCHKOOSCTZFNIHRICNUSRXADAQWKCGWKGFXNLXDLKQMIEDLAWHTSXDOQZPDIFPZ9999EMGRZ"
    "G9SXUO9LIBCHKOOSCTZFNIHRICNUSRXADAQWKCGWKGFXNLXDLKQMIEDLAWHTSXDOQZPDIFPZ99"
    "99TRINITY99999999999999999999OMGNKWYLE999999999MMMMMMMMMHFEMDIIFMSMH99QL9F"
    "EOENBUZYT";

/*
 * Bundle of size 2 - No value
 */
const char* TX_1_OF_2 =
    "9RVERIBISNTHFFNZBBXYPSFCFQKKSHVNQLTXWWLVDBVQJFRDYZVRUKIDGILWNU9JQPKVOHSLMA"
    "PVLQCYSVYQJFIIEVUXWZPUVRWZUGAZGKNQWIJVJTHHWFWMSWAPWPANCDAPPDELETUGUNIF9NZT"
    "VIOTLWPWZVMAH9YTBAHCGHIVICBQZWOQCIT9QKIHHIHBXHHDVNUGUIECVHAULJZFBZZTQKFQXN"
    "ISKCFLQKZQDNNKNDGAVWCMJFZZBVSVDCVWKVA9UDIHTGNDWNXPPZQIZJW9SDHYTTWHHU9GRQKL"
    "MMDXENMOMAKGBRGBMMLLZRHICDKZGQNEICAJ9W9MVYFQZEQVSSQKR9CBKRQEZU9UHHOABSIGKS"
    "JG9SMRTDWHZMUQXPXPRD9N9XXBYHHQVTDQKCAWEWPVANPAVYUMGLVWSUBKBKOXRPVOCOAHAIA9"
    "AHFAG9HRQVFQNVHEXAGVCPAQEIDWGOEVKRM9VDFFKAGBUKEIPFT9GPXHOOALEXWG9HEGCYRFRH"
    "BRJVBRQQGHPUBLE9QYHHXVAYNDOEKTKKHVDWN9YQCTYMZPLBAZLQLGRBETLBFZKDZOXEMUDWAN"
    "ZBILFQ9JGQL9GVUKACEWAAGHQPUZWSBAAMMXJVHKZSWQIXZWPRZPHUSBYIQFYKBEY9UBMBBRDY"
    "RMDXFFZKERHJMZBUHWZLYYREOQXFYAXVZEFCPSNTXZZWZNQXBYGQGQYLDPJBILPKSR9FLTHLHK"
    "TFAVYABMCGFXWDBXZVQXRL9UQGRQKKX9YMXCERLGFNZXMJASLPVULSUBCAKXU9DVGPCMAHKIMC"
    "SHGRDRYUXWFBGZG9ULYDAFEUABX9PHTORUWIDVEAGAZNKLQHTFUHPHDGYLJIPGABYZPXPJRVWQ"
    "DJLOBRSAFBINV9UBGVXJRXBPMDTQFPT9HYMAEDXEXLYEJJSHIQHLJHNI9NMXKPVGZCFKCOHGK9"
    "ILYWJYCJREENBOWEKZAEIVGIZXRHHUOCURHKEHVMZAPMMOAGXBVHDRCAP9JHRJJINELSECIZQI"
    "JOCESADQDXWBIOVUNWSHDWLKXRCAHZW9UIGYBAR9YHHVZNM9DYM9UWSEI9EPUSIZKOUSPADYIO"
    "X9CUKCULOUJUQFXNHIGBGMBHXC9VZPRVDTJASMGCALVXUGQCCAKBUIJJMVAQE9MTUHZETFJGTD"
    "JLEIXA9ILHCUIEBLNDNKLVBWGJDCFXFYUZJJ9TUBJXUDSJQDBTLLGXZGYUKDBQGCLQFYAPBZGE"
    "CTDAGAKJFRVDKXM9OFUFFBBOMUPUFWOSTVSKASCF9VWDUBTWNGBTTXIZXEVBTAS9LTOETNGNMU"
    "COXKPNVLDK9F9OYMN9OJZQHQHT9JGHBRYDINJKKPTDXJWLH9MNLJTLS9FSQIMIXYYIDQGKBQHY"
    "OQGZUWUHUWXG9C9LSYDTWRFFHDZM9DVYSNLA9AMNKOL9DAJPCFBFHZYZMDWSIFVGDLHETRINJJ"
    "9EVWMZRLCMFJYYSSJNXXMPB9ETLAWGWAZFGWUQVPOXEMKDIHYJXNXHT9N9ZJWTIAGAWPWD9MHQ"
    "GIWCUZXDCYRODRTYOVHTJK9UBQVCQPWWYJMHNPKLDYWKUHHYUTMKYRTWELLBOMVEQXYRCNPFZJ"
    "NV9XS9MSAEDXXMSEDJCAKJJ9KTBZVZFAPELSOJAVJUAVFCLOJBJGLXHZNFYUNLKWKVPIDIPXUX"
    "BVGQZSEYFIAZOSLEISHDR9DRCHLUUWXQIVBBXLLOSABEVRSNGMYSIWMOLYBRJGKYJDWNDBHLRW"
    "WORXGGQVPYYLUECMI9PZSFWOBFSZFYIRWFSHBGYZEFMDFGNQOZUEVWKIFXVBIISPSGAJGEHZRY"
    "XBNCETANAPDTAWPHBAZXZSFOGGXPGTIZIJRXNMFYATDBZ9RGFBFNCYDUFHI9RX9BFHZKJHVL9Q"
    "SITZWVISMQGJHWQGYHLMSOLMTGMTKZNUIMYJVHZ9PSNZDDOKNAXVYNGSIULJRJGEKKOJOLEGOD"
    "MPOEQOWD9FDCUISETBAOGFPYPDUIHGCMBPQDHOYRCEANSCNMFJJEIWTSPQSXNYJTHANJHJOAEE"
    "MRPXNYQJVECEQ9S9IGOFVLFRVKGUPPDAQT9CPJZILZJK9YAPXMNMEPXTORLAJIFPNNVZSTLAOL"
    "Y9AQTDKAHGSNAZKWIDLWBXZPJTXWVBFXCFWXTXRTZKPWCHICGJZXKE9GSUDXZYUAPLHAKAHYHD"
    "XNPHENTERYMMBQOPSQIDENXKLKCEYCPVTZQLEEJVYJZV9BWU99999999999999999999999999"
    "9EKVLA9999999999999999999999AJVWCZD99999999999A99999999SJLBISWQTONKVZPPDHL"
    "SPTWDETCNKGW9KNZ9NBDYDDAQSUBLZRWMFSUVECEARDHOJVWLTJ9VFYKVTQIUYLINC9GLMVBPC"
    "ZJHEXPQIBNNDBIROSBHXVBVOOPUWUDDAIIWLKPDKJMTEAQOSSZQQIDAHZXTEP9GAA9999JILXY"
    "HMATKDWMWYSMUAHZCSATXFJE9MOBTPLAQTZEDAOJQVB9ADQWIYRFLS9NB9MHC9ZY9KYBBMYA99"
    "99999999999999999999999999999999999999999999999999999999WURHSEBTKCN9ROCKER"
    "SLKTPOGSG";

const char* TX_2_OF_2 =
    "JCRFHRWXP99HEYQHLRKDDHDFWPKJXASWUKZK9VXAJYCNNHOHRCDNGWACLDNAO9ZURMXZCPKHNK"
    "RLUTDV9JPDAX9YUBHDIPMOPMYMTIZQOUUDV9JNS9GPGPUDBNRBPJUJWJRQHEHDKEDXVORHSDN9"
    "GTWVVHBDBCFRYRARUTPCOLUPYOWPEZGROB9IAGRDNNPRCRUVHENNCSJPHSAABAVFFRWY9XWDBN"
    "QCMATXNBETYSVVRQHBYBCHMLXZNGVFBXIIPHNRET9LUTFVDQYURORJIYSGJAMEDTZGYPVINHTZ"
    "W9CZTOQEXRFSWUUWLHMTNMHXACORTOLVAXSZNTQQNVINHQFDEY99IXVJPTHSHKWLTDOQYKUKCT"
    "OTLVX9PQFZW9ABYMNUOTTOHTGXEXXRDJMRZBWMLXTHGGVFVZTBGCAJKYFPZATZTOBOBSZAYK9Y"
    "OFYGWPEXVGWMJSJIRLBUUSSMUVMJCRUISYP9FYKDNNVTU9ZEQAPGEVXBMW9PZCMLAPTGHJB9JY"
    "ELADEZXHGAQHY9BBFMHBOXMZSPYSRHMOSMFXXKGEWHUMSIVHSFSBDTMTGRNCNKAPXUB9TYLENS"
    "DNIQIR9ORQEWEHSNNU9MNV9FLIEUJHHULODQIQNMWYDNSUSPVLQSAPGWKIDY9WTMSMZVKYHVLU"
    "HQW9NFYBVIOUNIUTPLRCOKODNQZBHXMTNH9VIENJDQMSWOSLL9WTEAHVE9NCOFINSOZUALSU9K"
    "WIBYISXZFTVWYMJEOXVOSTDNQKDLSPHKHPCWGBMVNEGCPHUQZ99X9XFKKZEPNQLCAHINOHZKZX"
    "UCRANNPASINKODC9YCKVMETVSTBAINUROLVEVWZVPS9OCPHDOWICHWOAMPTUMXCDMHVIA9RQRE"
    "PYDDAQOBZEDKTGMPKERGAZJEZBLVPAIHCYMNEMGSCMKFIDZJKWBKPNPGFFOXEEIKTWWXUSVJRD"
    "UCRKRBXXQYFIXHNTTZBRVCNSIFYUXWTELGY9YNPNZWVROIXCYHVNCJLHHRZNQJMBTITWDCDLPB"
    "CIQZLSZATJHHDMZFCPCYDEJJATSXUO9LNTIRSTJWIRULLCNFZUGI9GWSLGAEYOHBZQIPMAIMWY"
    "KAIUHNOPIBPLNZTTJVVGKDNHZNMYPEXHNTDOCRUYJIQKP9RQAGWSIZJXCRRMOFQBUSSTFYHCSH"
    "E9FCEMMC9YGPQFH9DWTMRGYILJCYCGKNMTMAZMITQRPWOQKTZJTSOYXTTXKIGZFKFVBR9VQBCF"
    "SCWJWJRWBYFIOJKILUDWQIAGJASFMBUDMVZPEHUVJMN9XP9XNJNRBCMYMCQTRKOPB9UM9D9VHW"
    "PT9VXNBOGGRRHCYCZPHUNCWBDACYQYUYQCPTVAURYAGGKNX9RYEUCTKXWLTH9HX9NBNHIMPWVC"
    "WUINAFV9LY9BVIJZWUVUXQVNBGAXOC9XLHNES9VBRN9P9AL9NPSZZQIHTNGTGRFVCUDTTSSOUU"
    "VIRLTXKADOGCWOQZBNZKCNGAAQWTHZDYHLJRDWKKPXVMVMOAMSHMIXFCXFFRBXSFUUKOAHKAPH"
    "TIIDMYEAZYGKFVJBSDJPVVOQLRWCQEDECG99RARUYJYXIMAEVFRWJ9SJFUSNN9KZGR99999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "9999999999999999999999999999AJVWCZD99A99999999A99999999SJLBISWQTONKVZPPDHL"
    "SPTWDETCNKGW9KNZ9NBDYDDAQSUBLZRWMFSUVECEARDHOJVWLTJ9VFYKVTQIUYJILXYHMATKDW"
    "MWYSMUAHZCSATXFJE9MOBTPLAQTZEDAOJQVB9ADQWIYRFLS9NB9MHC9ZY9KYBBMYA9999JILXY"
    "HMATKDWMWYSMUAHZCSATXFJE9MOBTPLAQTZEDAOJQVB9ADQWIYRFLS9NB9MHC9ZY9KYBBMYA99"
    "999999999999999999999999999999999999999999999999999999999MUXOEKO9RWMUNJGFR"
    "DOOYVXTMC";

/// The Trunk of the last indexed transaction in bundle
const char* BUNDLE_OF_2_TRUNK_TX =
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
    "99999999999999999999999999999999999999999ABTRPTOCQGBQXGMLHYDIUEPPXNFMJXBXG"
    "JSOOYXFFFU9LJFPDZILIEZHRADFIBWAR9NFXEUBZTXCCPMXD99999999999999999999999999"
    "99999999999999999999999999999JVWCZD99999999999999999999LZTYZETLZXECBNAQAVZ"
    "CDEFWGLZMQVEPCVNOLDHPTGVKTLCGVGKPNAKPTUHNLVBJQMUBUMZZBRXTB9JNXXIYUWIHSSQXE"
    "ZXCWAMQUEYUVSLE9XLDILHP9DJWHOGTWCLRRUGILOHJRZJJALNPEYCSBLKKI9UQUZ9999BM9RE"
    "ZHRDWSUMXTJNTUKLQOZUGCK9GWOSF9YWNGM9TBZGIWEPWTQJXDMCMNVRNZECAH9SUYNYPZC999"
    "99999999999999999999999999999999999999999999999999999999RRPFLWHSFWFLIOWHEN"
    "IPCTOKOHD";

void populate_bundle_of_transactions(tangle_t* const tangle,
                                     bundle_transactions_t* bundle,
                                     size_t num_of_trnasactions);

#ifdef __cplusplus
}
#endif

#endif  //__CONSENSUS_TEST_UTILS_BUNDLE_H__
