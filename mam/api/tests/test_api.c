/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/trinary/tryte_ascii.h"
#include "mam/api/api.h"
#include "mam/mam/mam_channel_t_set.h"
#include "mam/ntru/mam_ntru_sk_t_set.h"
#include "mam/test_utils/test_utils.h"

static mam_api_t api;
static mam_channel_t *cha = NULL, *ch1a = NULL;
static mam_endpoint_t *epa = NULL, *ep1a = NULL;

static tryte_t API_SEED[] =
    "APISEEDAPISEEDAPISEEDAPISEEDAPISEEDAPISEEDAPISEEDAPISEEDAPISEEDAPISEEDAPIS"
    "EEDAPI9";

#define PAYLOAD                                                                \
  "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec vel tortor " \
  "semper, lacinia augue eu, interdum ligula. Duis nec nulla arcu. Sed "       \
  "commodo sem urna, vitae tristique mauris tempor at. Nullam vel faucibus "   \
  "enim. In vel maximus urna, nec pretium ipsum. Nam vitae augue consequat, "  \
  "lacinia lacus eget, maximus ante. Quisque egestas nibh ut sem molestie, "   \
  "at ornare odio congue. In turpis libero, euismod eget euismod eget, "       \
  "interdum pretium orci. Nulla facilisi. In faucibus, purus non "             \
  "pellentesque ultrices, nisi urna finibus leo, id convallis lacus ante "     \
  "quis nisi. Suspendisse vehicula est ipsum, at feugiat est congue vitae. "   \
  "Duis id tincidunt purus. Ut ornare leo vel venenatis suscipit. Mauris "     \
  "pulvinar sapien sit amet velit varius, quis condimentum orci lobortis. "    \
  "Praesent eget est porta, rutrum eros eu, porta eros. Donec et felis a "     \
  "metus imperdiet imperdiet. Cras a fringilla mi. Aliquam auctor dapibus "    \
  "dolor, quis euismod augue condimentum ut. Curabitur a nisl id purus "       \
  "efficitur tincidunt dapibus non arcu. Integer efficitur faucibus massa. "   \
  "Ut vitae ultrices ex. In hendrerit tincidunt lacus, nec mollis urna "       \
  "iaculis vitae. Aliquam volutpat sodales augue, sed posuere eros lobortis "  \
  "vel. Cras rutrum elit nec gravida volutpat. Aliquam erat volutpat. Cras "   \
  "dignissim tellus eget magna dapibus, in finibus ipsum facilisis. "          \
  "Curabitur eleifend elit nibh, eu egestas tortor consequat ac. Nam "         \
  "facilisis dolor sit amet lacus elementum, vitae porta libero iaculis. "     \
  "Duis nisl diam, dignissim eu luctus pellentesque, mattis ac magna. Sed "    \
  "malesuada dignissim orci vitae cursus. Pellentesque viverra massa vitae "   \
  "neque tincidunt ornare. Phasellus iaculis lobortis sapien eu interdum. "    \
  "Aenean facilisis a enim a malesuada. Maecenas ut arcu bibendum, interdum "  \
  "odio ac, hendrerit est. Quisque dui diam, venenatis non purus eget, "       \
  "volutpat facilisis nisi. Nullam mi urna, imperdiet eu eros tincidunt, "     \
  "congue dapibus velit. Donec non eleifend nunc, sed sodales nisl. Aliquam "  \
  "tristique pulvinar ante, nec tincidunt ex. Fusce eget porta arcu. Cras "    \
  "quis ornare magna, ac ullamcorper dolor. Etiam hendrerit posuere "          \
  "hendrerit. Donec sit amet fermentum quam, non euismod dui. Sed suscipit "   \
  "posuere est ac tempor. Vivamus id aliquam urna. Integer cursus, tellus "    \
  "nec malesuada varius, lectus justo maximus est, ac molestie dolor nulla "   \
  "ut mauris. Suspendisse molestie ornare consequat. Nullam convallis ex eu "  \
  "tortor pretium pulvinar. Cras in gravida augue, a porttitor libero. Morbi " \
  "orci sapien, egestas at consectetur non, egestas ut orci. Praesent at "     \
  "nisl sollicitudin, elementum nunc sit amet, placerat urna. Aliquam ut "     \
  "felis auctor, scelerisque mi ut, posuere arcu. Donec venenatis ac ex eget " \
  "suscipit. Quisque ut nibh vitae lacus interdum pretium ut ac tellus. Sed "  \
  "cursus dui vel fringilla porta. Suspendisse in rhoncus lectus. Ut "         \
  "interdum risus at consequat porta. Donec commodo accumsan magna quis "      \
  "ornare. Pellentesque habitant morbi tristique senectus et netus et "        \
  "malesuada fames ac turpis egestas. Donec sed mollis purus, eget viverra "   \
  "ipsum. Mauris vulputate rutrum leo a lobortis. Mauris vitae semper nibh. "  \
  "Maecenas vestibulum justo libero. Duis ornare nunc nec dolor auctor "       \
  "congue. Phasellus a quam ultricies, pharetra purus ac, pretium ipsum. "     \
  "Pellentesque dignissim nulla at dolor ullamcorper, at aliquam nunc "        \
  "cursus. Proin in maximus ligula, sed imperdiet nunc. Proin eu ante neque. " \
  "Ut consectetur ultricies ipsum, et laoreet sapien iaculis rutrum. Sed "     \
  "ultrices magna metus, in volutpat felis convallis lobortis. Cras "          \
  "elementum mauris ut nibh tincidunt, sed venenatis ante lacinia. Nunc "      \
  "ullamcorper ante sit amet congue vulputate. Vivamus magna odio, molestie "  \
  "ut risus ac, lobortis vehicula mauris. Proin bibendum id sapien id "        \
  "dictum. Duis eu porta elit. Nullam auctor, diam eu sagittis pretium, "      \
  "nulla ligula pharetra ligula, in blandit tortor mi in nisl. Donec "         \
  "accumsan dolor orci, a lacinia lectus sollicitudin a. Sed odio eros, "      \
  "mattis eget lorem fringilla, egestas tempus turpis. Orci varius natoque "   \
  "penatibus et magnis dis parturient montes, nascetur ridiculus mus. In hac " \
  "habitasse platea dictumst. Nulla facilisi. Fusce vel dolor accumsan, "      \
  "sagittis velit vitae, pretium erat. Etiam tempor erat quis neque "          \
  "vulputate, vel convallis velit porttitor. Etiam risus urna, consectetur "   \
  "et justo ut, accumsan consectetur est. Proin eu eros et felis ultrices "    \
  "suscipit eget et enim. Mauris sem neque, blandit sed mollis eget, feugiat " \
  "non mauris. Etiam pellentesque est nulla, ac ornare elit vulputate et. "    \
  "Donec quis cursus lacus. Donec scelerisque tincidunt massa at rutrum. "     \
  "Quisque commodo erat non cursus euismod. Pellentesque vitae luctus sem, "   \
  "ac volutpat quam. Aliquam vitae consectetur nibh. Nunc eu pharetra velit. " \
  "Praesent at tincidunt felis. Aenean non ultrices neque. Duis finibus urna " \
  "ut est rhoncus pharetra. Curabitur commodo eget sem in posuere. Praesent "  \
  "at blandit purus. In pretium urna vel purus commodo, ac sollicitudin odio " \
  "pretium. Praesent sit amet magna id mi tincidunt ultrices et vel lectus. "  \
  "Aenean nec finibus est. Etiam eu purus ac ligula auctor malesuada a eget "  \
  "purus. Curabitur convallis, ligula ut tristique rutrum, urna purus "        \
  "pulvinar risus, a suscipit ante est ac eros. In ut dignissim mi. Aenean "   \
  "vel posuere tellus. Phasellus eu sem in nunc malesuada aliquet. In at "     \
  "ullamcorper ipsum. Aliquam erat volutpat. Pellentesque sagittis ligula "    \
  "non venenatis mollis. Curabitur non cursus nulla. Phasellus vestibulum "    \
  "venenatis sapien vel imperdiet. Integer id ultricies sem, id fringilla "    \
  "dui. Aenean id massa id tortor finibus euismod id at diam. Integer "        \
  "aliquam est metus, ac aliquam lacus auctor ut. Quisque vel convallis mi. "  \
  "Sed ultrices nisi nec ipsum tempus blandit. Aenean at cursus justo, nec "   \
  "feugiat erat. Etiam mollis in metus ac dignissim. Vestibulum sed massa "    \
  "arcu. Nam condimentum id orci molestie ornare. Quisque semper non orci a "  \
  "pharetra. Praesent id interdum massa. Morbi non porta libero. Nulla "       \
  "dignissim purus enim, et elementum velit feugiat eu. Fusce dapibus est "    \
  "eget dolor dignissim, vitae placerat risus condimentum. Praesent "          \
  "tincidunt efficitur massa, porttitor mollis velit rhoncus id. Nullam "      \
  "dignissim, nisi sed placerat faucibus, massa mauris ullamcorper ipsum, "    \
  "nec tristique velit tellus quis tortor. Vestibulum ante ipsum primis in "   \
  "faucibus orci luctus et ultrices posuere cubilia Curae; Donec a urna dui. " \
  "Sed et augue eget dolor efficitur volutpat sed a turpis. Integer cursus, "  \
  "arcu vitae aliquam fermentum, lectus metus elementum elit, vitae suscipit " \
  "felis sapien tristique erat. Suspendisse et dui magna. Aenean pharetra "    \
  "augue augue. Nam eu eros sed tortor hendrerit venenatis at nec sem. Etiam " \
  "in risus ac dolor lobortis ornare. Orci varius natoque penatibus et "       \
  "magnis dis parturient montes, nascetur ridiculus mus. Mauris pellentesque " \
  "nibh ut imperdiet eleifend. Nam rhoncus rhoncus porta. Ut sed "             \
  "pellentesque quam. Pellentesque lobortis convallis tincidunt. Praesent "    \
  "non venenatis sapien. Nulla facilisi. Phasellus egestas tristique "         \
  "sagittis. Fusce volutpat tristique rhoncus. Duis semper ipsum sed "         \
  "volutpat lobortis. Suspendisse ut eros nunc. Pellentesque eu felis et "     \
  "tortor cursus dictum. Aliquam viverra augue a fermentum lobortis. "         \
  "Phasellus viverra placerat velit sed aliquet. Duis id rutrum sem. Vivamus " \
  "feugiat placerat ipsum at vehicula. Ut sit amet metus bibendum libero "     \
  "semper cursus. Integer in quam eu leo interdum eleifend. Cras tristique "   \
  "eros vel fermentum vestibulum. In hac habitasse platea dictumst. Donec id " \
  "libero ut elit tempor vestibulum. Nullam vel porttitor dolor. In pretium "  \
  "bibendum posuere. Donec congue scelerisque hendrerit. Ut sed tincidunt "    \
  "enim. Ut eleifend lectus sem. Vestibulum accumsan euismod purus ac "        \
  "lacinia. Morbi ornare iaculis nulla, eu ultrices ante aliquam sit amet. "   \
  "Phasellus tincidunt vehicula viverra. Mauris ut massa nulla. Maecenas sit " \
  "amet velit in justo blandit mattis eu quis massa. Sed vel tincidunt "       \
  "metus. Curabitur euismod erat ac ante consectetur interdum. Aliquam erat "  \
  "volutpat. Duis sem arcu, hendrerit in quam vitae, molestie vehicula "       \
  "lorem. Morbi sed sagittis dui, vel gravida diam. In nulla ante, pharetra "  \
  "vel leo sit amet, maximus commodo metus. Integer posuere nisi nisi, eu "    \
  "dapibus nisl viverra ac. Etiam euismod sapien quis justo laoreet "          \
  "lobortis. Donec sagittis est vel tincidunt blandit. Sed ac tortor eu eros " \
  "gravida ultricies. Nam id neque sed elit pharetra rutrum. Suspendisse "     \
  "maximus varius urna quis eleifend. Donec dictum finibus interdum. "         \
  "Maecenas lectus risus, fringilla vel ipsum eget, porttitor mattis magna. "  \
  "Fusce massa nulla, bibendum ut dictum nec, congue vitae lectus. Sed ac "    \
  "pharetra ante. Integer sed nisi sem. Sed mattis pretium porta. Donec in "   \
  "efficitur urna. In placerat, justo at ultrices tempor, tortor turpis "      \
  "bibendum massa, ut maximus augue odio a felis. Curabitur suscipit erat eu " \
  "lectus porttitor sodales. Nam facilisis placerat commodo. Integer vel "     \
  "odio ut dui faucibus sollicitudin. Suspendisse nec ultrices turpis. "       \
  "Vestibulum nisl magna, mollis nec porta at, facilisis nec erat. Interdum "  \
  "et malesuada fames ac ante ipsum primis in faucibus. Sed id dui in mi "     \
  "dictum ullamcorper. Curabitur purus est, pretium ut urna et, posuere "      \
  "laoreet dolor. Mauris blandit felis non massa cursus, et luctus turpis "    \
  "imperdiet. Vestibulum ante ipsum primis in faucibus orci luctus et "        \
  "ultrices posuere cubilia Curae; Praesent imperdiet, leo id vehicula "       \
  "viverra, mi diam pellentesque dui, sed faucibus augue nisi id orci. "       \
  "Suspendisse in sagittis metus, vitae vestibulum odio. Morbi tempus ex "     \
  "massa, consequat aliquam turpis tristique vitae. Quisque accumsan "         \
  "interdum sagittis. Integer eget malesuada nulla, id blandit mauris. "       \
  "Integer elementum quam et leo pretium sodales. Fusce tempus venenatis "     \
  "fermentum. Nulla lobortis eros molestie risus cursus, ac suscipit lorem "   \
  "fermentum. Etiam a augue ac quam accumsan ultrices eu id leo. Mauris "      \
  "laoreet nulla non magna vestibulum, et ullamcorper eros suscipit. Etiam "   \
  "hendrerit purus et ex porta tempus. Vestibulum ac lorem quis arcu semper "  \
  "feugiat. Aenean ac orci ut urna rhoncus pretium. Duis condimentum non "     \
  "mauris sit amet semper. Nunc vehicula elit scelerisque lectus vulputate, "  \
  "eget imperdiet lectus tincidunt. Mauris feugiat dolor imperdiet rhoncus "   \
  "vehicula. Etiam sit amet suscipit augue. Morbi elit sapien, convallis ut "  \
  "tristique in, pharetra vitae ligula. Praesent volutpat, enim facilisis "    \
  "auctor tincidunt, est erat fringilla nunc, nec consectetur lectus augue a " \
  "dolor. Donec interdum, eros eget lacinia ultricies, risus ligula posuere "  \
  "tellus, non maximus sem nibh quis lacus. Donec vehicula porttitor dictum. " \
  "Proin fringilla, enim quis auctor tempor, mi leo rhoncus ipsum, id "        \
  "dapibus est libero vel libero. Etiam nisl erat, cursus eu eros vel, "       \
  "vestibulum scelerisque risus. Proin in arcu pellentesque, pretium nisl "    \
  "sit amet, rhoncus lectus. Etiam id tortor ac erat malesuada convallis "     \
  "eget a enim. Etiam varius dolor nec feugiat porta. Etiam rhoncus suscipit " \
  "ligula, sit amet gravida justo faucibus nec. In mollis finibus purus. "     \
  "Integer sit amet massa a nisl malesuada viverra nec et sem. Sed placerat "  \
  "neque at commodo porttitor. Sed eget nunc sit amet magna tincidunt "        \
  "mattis. Pellentesque habitant morbi tristique senectus et netus et "        \
  "malesuada fames ac turpis egestas. In mollis ante auctor, sagittis felis "  \
  "rhoncus, sodales est. Nulla non lacus tempor, consectetur enim at, semper " \
  "magna. Donec felis odio, finibus ac sollicitudin ut, pretium non felis. "   \
  "Nunc in venenatis leo. Curabitur fringilla neque lacus, sit amet "          \
  "consectetur risus luctus sed. Donec convallis vel enim dignissim iaculis. " \
  "Nulla tristique non ipsum vel faucibus. Nulla suscipit tortor nec felis "   \
  "malesuada porta. Nam elit risus, auctor eu enim sit amet, pharetra "        \
  "efficitur erat. Vestibulum vel quam arcu. Sed vehicula consequat auctor. "  \
  "Quisque blandit massa sit amet ipsum fringilla, et congue ipsum pulvinar. " \
  "Aenean vel mauris justo. Nam non molestie mauris. Quisque ut arcu sit "     \
  "amet nisl congue pretium. Suspendisse facilisis augue ut sapien vehicula "  \
  "iaculis. Proin pretium ornare erat, vitae suscipit risus bibendum non. "    \
  "Integer viverra quam dui, vel mattis orci hendrerit eu. Integer aliquam "   \
  "egestas mattis. Phasellus hendrerit id metus sit amet cursus. In hac "      \
  "habitasse platea dictumst. Mauris consectetur, felis convallis ultrices "   \
  "pellentesque, eros magna viverra est, a interdum metus dui vitae ante. "    \
  "Vivamus eget rhoncus dui. Suspendisse luctus, nunc et sagittis viverra, "   \
  "orci orci feugiat lectus, ut suscipit tellus risus id odio. Duis porta "    \
  "nibh ac fermentum interdum. Praesent sed felis eleifend, porttitor eros "   \
  "id, ornare nisl. Ut sit amet pharetra odio. Pellentesque ac mi viverra, "   \
  "congue lacus vel, eleifend sapien. Etiam gravida ipsum ut erat "            \
  "vestibulum, nec viverra velit gravida. Donec non eleifend turpis, a "       \
  "tempor leo. Duis sollicitudin ligula vel pharetra finibus. Vestibulum "     \
  "justo lectus, tempor eget porttitor et, gravida ac felis. Praesent dictum " \
  "sem sed ornare interdum. Mauris vel tellus blandit, mattis tellus at, "     \
  "ultricies neque. Nullam sit amet lectus augue. Nunc sed ultricies ipsum. "  \
  "Nullam risus ligula, rutrum ullamcorper magna nec, tempus viverra lorem. "  \
  "Donec laoreet felis sed erat elementum mattis. Integer accumsan turpis "    \
  "nec egestas consequat. Quisque molestie augue vitae magna rhoncus mollis. " \
  "Morbi at justo in ipsum imperdiet consectetur non et urna. Nam dapibus ex " \
  "quis ornare maximus. Etiam sit amet libero molestie."

#define TEST_CHANNEL_NAME "CHANAME"
#define TEST_CHANNEL_1_NAME "CHANAME9"
#define TEST_END_POINT_NAME "EPANAME"
#define TEST_END_POINT_1_NAME "EPANAME9"
#define TEST_PRE_SHARED_KEY_A_STR "PSKIDAPSKIDAPSKIDAPSKIDAPSK"
#define TEST_PRE_SHARED_KEY_A_NONCE_STR "PSKANONCE"
#define TEST_PRE_SHARED_KEY_B_STR "PSKIDBPSKIDBPSKIDBPSKIDBPSK"
#define TEST_PRE_SHARED_KEY_B_NONCE_STR "PSKBNONCE"
#define TEST_NTRU_NONCE "NTRUBNONCE"
#define TEST_MSS_DEPTH 1

static void test_api_write_header(
    mam_api_t *const api, mam_psk_t const *const pska,
    mam_psk_t const *const pskb, mam_ntru_pk_t const *const ntru_pk,
    mam_msg_pubkey_t pubkey, mam_msg_keyload_t keyload,
    mam_channel_t *const cha, mam_endpoint_t *const epa,
    mam_channel_t *const ch1a, mam_endpoint_t *const ep1a,
    bundle_transactions_t *const bundle, trit_t *const msg_id) {
  mam_channel_t *ch = cha;
  mam_endpoint_t *ep = NULL;
  mam_channel_t *ch1 = NULL;
  mam_endpoint_t *ep1 = NULL;
  mam_psk_t_set_t psks = NULL;
  mam_ntru_pk_t_set_t ntru_pks = NULL;

  if (mam_msg_pubkey_epid == pubkey) {
    ep = epa;
  } else if (mam_msg_pubkey_chid1 == pubkey) {
    ch1 = ch1a;
  } else if (mam_msg_pubkey_epid1 == pubkey) {
    ep1 = ep1a;
  }

  if (mam_msg_keyload_psk == keyload) {
    TEST_ASSERT(mam_psk_t_set_add(&psks, pska) == RC_OK);
    TEST_ASSERT(mam_psk_t_set_add(&psks, pskb) == RC_OK);
  } else if (mam_msg_keyload_ntru == keyload) {
    TEST_ASSERT(mam_ntru_pk_t_set_add(&ntru_pks, ntru_pk) == RC_OK);
  }

  TEST_ASSERT(mam_api_bundle_write_header(api, ch, ep, ch1, ep1, psks, ntru_pks,
                                          0, bundle, msg_id) == RC_OK);

  mam_psk_t_set_free(&psks);
  mam_ntru_pk_t_set_free(&ntru_pks);
}

static void test_api_write_packet(
    mam_api_t *const api, bundle_transactions_t *const bundle,
    trit_t *const msg_id, mam_msg_pubkey_t pubkey, mam_msg_checksum_t checksum,
    mam_channel_t *const cha, mam_endpoint_t *const epa,
    mam_channel_t *const ch1a, mam_endpoint_t *const ep1a, char const *payload1,
    bool is_last_packet) {
  tryte_t *payload_trytes = NULL;

  payload_trytes = (tryte_t *)malloc(2 * strlen(payload1) * sizeof(tryte_t));
  ascii_to_trytes(payload1, payload_trytes);

  TEST_ASSERT(mam_api_bundle_write_packet(api, msg_id, payload_trytes,
                                          2 * strlen(payload1), checksum,
                                          bundle, is_last_packet) == RC_OK);

  free(payload_trytes);
}

static void test_api_read_msg(mam_api_t *const api,
                              bundle_transactions_t *const bundle,
                              mam_psk_t const *const pre_shared_key,
                              mam_ntru_sk_t const *const ntru,
                              mam_channel_t *const cha, char **payload2,
                              bool *const is_last_packet) {
  tryte_t *payload_trytes = NULL;
  size_t payload_size = 0;

  TEST_ASSERT(mam_api_bundle_read(api, bundle, &payload_trytes, &payload_size,
                                  is_last_packet) == RC_OK);

  *payload2 = (char *)calloc(payload_size / 2 + 1, sizeof(char));
  trytes_to_ascii(payload_trytes, payload_size, *payload2);
  free(payload_trytes);
}

static void test_api_create_channels(mam_api_t *api, mam_channel_t **const cha,
                                     mam_channel_t **const ch1,
                                     mam_endpoint_t **const epa,
                                     mam_endpoint_t **ep1) {
  mss_mt_height_t d = TEST_MSS_DEPTH;

  // TODO usse api methods
  /* create channels */
  {
    trits_t cha_name = trits_alloc(3 * strlen(TEST_CHANNEL_NAME));
    trits_from_str(cha_name, TEST_CHANNEL_NAME);

    *cha = malloc(sizeof(mam_channel_t));
    TEST_ASSERT(0 != *cha);
    memset(*cha, 0, sizeof(mam_channel_t));
    TEST_ASSERT(mam_channel_create(&api->prng, d, cha_name, *cha) == RC_OK);

    /* create endpoints */
    {
      trits_t epa_name = trits_alloc(3 * strlen(TEST_END_POINT_NAME));
      trits_from_str(epa_name, TEST_END_POINT_NAME);

      *epa = malloc(sizeof(mam_endpoint_t));
      TEST_ASSERT(0 != *epa);
      memset(*epa, 0, sizeof(mam_endpoint_t));
      TEST_ASSERT(mam_endpoint_create(&api->prng, d, (*cha)->name, epa_name,
                                      *epa) == RC_OK);
      trits_free(epa_name);
    }
    {
      trits_t ep1a_name = trits_alloc(3 * strlen(TEST_END_POINT_1_NAME));
      trits_from_str(ep1a_name, TEST_END_POINT_1_NAME);

      *ep1 = malloc(sizeof(mam_endpoint_t));
      TEST_ASSERT(0 != *ep1);
      memset(*ep1, 0, sizeof(mam_endpoint_t));
      TEST_ASSERT(mam_endpoint_create(&api->prng, d, (*cha)->name, ep1a_name,
                                      *ep1) == RC_OK);
      trits_free(ep1a_name);
    }
    {
      trits_t ch1a_name = trits_alloc(3 * strlen(TEST_CHANNEL_1_NAME));
      trits_from_str(ch1a_name, TEST_CHANNEL_1_NAME);

      *ch1 = malloc(sizeof(mam_channel_t));
      TEST_ASSERT(0 != *ch1);
      memset(*ch1, 0, sizeof(mam_channel_t));
      TEST_ASSERT(mam_channel_create(&api->prng, d, ch1a_name, *ch1) == RC_OK);
      trits_free(ch1a_name);
    }
    trits_free(cha_name);
  }
}

static void test_api_generic() {
  retcode_t e = RC_OK;
  bundle_transactions_t *bundle = NULL;
  trit_t msg_id[MAM_MSG_ID_SIZE];
  char *payload2 = NULL;
  mam_ntru_sk_t ntru[1];
  mam_psk_t pska[1], pskb[1];

  /* gen recipient'spongos ntru keys, public key is shared with sender */
  {
    MAM_TRITS_DEF0(ntru_nonce, 30);
    ntru_nonce = MAM_TRITS_INIT(ntru_nonce, 30);
    trits_from_str(ntru_nonce, TEST_NTRU_NONCE);
    TEST_ASSERT(ntru_init(ntru) == RC_OK);
    ntru_gen(ntru, &api.prng, ntru_nonce);
    TEST_ASSERT(mam_api_add_ntru_sk(&api, ntru) == RC_OK);
  }

  /* gen psk */
  {
    trits_from_str(mam_psk_id(pska), TEST_PRE_SHARED_KEY_A_STR);
    prng_gen_str(&api.prng, MAM_PRNG_DST_SEC_KEY,
                 TEST_PRE_SHARED_KEY_A_NONCE_STR, mam_psk_key(pska));
    trits_from_str(mam_psk_id(pskb), TEST_PRE_SHARED_KEY_B_STR);
    prng_gen_str(&api.prng, MAM_PRNG_DST_SEC_KEY,
                 TEST_PRE_SHARED_KEY_B_NONCE_STR, mam_psk_key(pskb));
    TEST_ASSERT(mam_api_add_psk(&api, pskb) == RC_OK);
  }

  bool is_last_packet;

  /* chid=0, epid=1, chid1=2, epid1=3*/
  for (mam_msg_pubkey_t pubkey = 0; (int)pubkey < 4; ++pubkey) {
    /* public=0, psk=1, ntru=2 */
    for (mam_msg_keyload_t keyload = 0; (int)keyload < 3; ++keyload) {
      /* none=0, mac=1, mssig=2 */
      for (mam_msg_checksum_t checksum = 0; (int)checksum < 3; ++checksum) {
        bundle_transactions_new(&bundle);

        /* write header and packet */
        test_api_write_header(&api, pska, pskb, &ntru->public_key, pubkey,
                              keyload, cha, epa, ch1a, ep1a, bundle, msg_id);
        test_api_write_packet(&api, bundle, msg_id, pubkey, checksum, cha, epa,
                              ch1a, ep1a, PAYLOAD, true);

        /* read header and packet */
        test_api_read_msg(&api, bundle, pskb, ntru, cha, &payload2,
                          &is_last_packet);
        TEST_ASSERT(is_last_packet);
        TEST_ASSERT_EQUAL_STRING(PAYLOAD, payload2);

        /* cleanup */
        bundle_transactions_free(&bundle);
        free(payload2);
        payload2 = NULL;
      }
    }
  }

  TEST_ASSERT(e == RC_OK);
}

static void test_api_multiple_packets() {
  bundle_transactions_t *bundle = NULL;
  trit_t msg_id[MAM_MSG_ID_SIZE];
  tryte_t *payload_in = (tryte_t *)
      "RYFJ9ZCHZFYZSHCMBJPDHLQBODCMRMDH9CLGGVWJCZRJSNDWBTMWSBPYPFIIOLXEMKSJ9LJO"
      "AFBFPJL9XMGGZXFCZHFDLOLODLMLNERWGUBXUJCMHJXWJPGX9R9HUPIHEIPNNMXTULSMHJGP"
      "TDKYEFZ9FDKFOBBQS9QZ9LODBRQMVCKYIUXIPWMDNPSZEK9ZTDCJEFEQEAJAEABJQWPVCGSM"
      "WEZLLSDYWLWQPRENHOTDQYP9QTIULYOE9OIVEWUXTMYUMPTMHOYFRJMWJUKM9QAJKVQW9ADY"
      "TZFQTNDISJYNTSFIEESDZJJJUPBDJKNEYNMOIZOKFCARBCXVFTPQZEKVZBNZOSKMRHAJXG9U"
      "ZBNEUXM9LARLTSRQXYACOVOCIFHUETWFXXOLSSQSKNFUANYIGVMXTOZYSYBIXTRTYOWQRVTF"
      "VMMXSH9WVQKEYRALLTBJIBYJIMTV99PATCFBKXZPLIBPNQZYJLDUXRWKPRRJTPKWQAQFFQWS"
      "VAHUOAAOWJLSYVYLI99RNUONKEEJYFIMEWBIKLGARGTABJDCHKQM9LFFMKQXHFSCGJXAYCLF"
      "RFLWPKPPNHOWIMEFNRCGNDCHMEYYJWHPRJOYOFFPNISVUNMVYFW9ECUZBDOSUCFZPOREJMND"
      "ZMZYWUBBFICWJ9IYHJDIDGLPERWCYXMFHXZGNLWXOCXBGEWZFKITEUMEVNUWLRUMHEZUJMRI"
      "TTNKN9PBUR9MOZINMWWTRXVRRZHQVP9QDJPGBZALBVI9GXNZYQTOPKDJPXPLADTBUNRQFLTE"
      "Q9XLMEPTJUWYIGNQMMLECGXAQOSFMDWFBFUYB9FEUMXSCRQVQMT9E9CEPRVQWQFVWT9UC9FH"
      "NTCCRUHOOWXORIRHNNZUOQCSOGJCRUWCQHCLZMRNIWUESDEQWPHLLNEHXFDLRUEOTLQERNPT"
      "OHNGGXIWJCKGKEGRFXYFLVOQVYQOVZ9QWGGBGZBLPVNQOBA9VYGKZE9MQYOHDKNE";
  size_t payload_in_size = strlen((char *)payload_in);
  tryte_t *payload_out = NULL;
  size_t payload_out_size = 0;
  bool is_last_packet;

  // write and read header
  {
    bundle_transactions_new(&bundle);
    TEST_ASSERT(mam_api_bundle_write_header(&api, cha, NULL, NULL, NULL, NULL,
                                            NULL, 0, bundle, msg_id) == RC_OK);
    TEST_ASSERT(mam_api_bundle_read(&api, bundle, &payload_out,
                                    &payload_out_size,
                                    &is_last_packet) == RC_OK);
    TEST_ASSERT(payload_out == NULL);
    TEST_ASSERT(payload_out_size == 0);
    bundle_transactions_free(&bundle);
  }

  // write and read packets
  const size_t num_packets = 256;
  for (size_t i = 0; i < num_packets; i++) {
    bundle_transactions_new(&bundle);
    TEST_ASSERT(mam_api_bundle_write_packet(
                    &api, msg_id, payload_in, payload_in_size,
                    (mam_msg_checksum_t)(i % 3), bundle,
                    i == (num_packets - 1) ? true : false) == RC_OK);
    TEST_ASSERT(mam_api_bundle_read(&api, bundle, &payload_out,
                                    &payload_out_size,
                                    &is_last_packet) == RC_OK);
    if (i < num_packets - 1) {
      TEST_ASSERT(!is_last_packet);
    } else {
      TEST_ASSERT(is_last_packet);
    }

    TEST_ASSERT_EQUAL_MEMORY(payload_in, payload_out, payload_in_size);
    free(payload_out);
    payload_out = NULL;
    payload_out_size = 0;
    bundle_transactions_free(&bundle);
  }
}

static void test_api_serialization() {
  mam_api_t deserialized_api;
  size_t serialized_size = mam_api_serialized_size(&api);
  MAM_TRITS_DEF0(buffer, serialized_size);
  buffer = MAM_TRITS_INIT(buffer, serialized_size);
  mam_api_serialize(&api, &buffer);
  buffer = trits_pickup_all(buffer);
  TEST_ASSERT((mam_api_deserialize(&buffer, &deserialized_api) == RC_OK));

  TEST_ASSERT_EQUAL_MEMORY(&deserialized_api.prng, &api.prng,
                           MAM_PRNG_KEY_SIZE);
  TEST_ASSERT_TRUE(
      mam_ntru_sk_t_set_cmp(&deserialized_api.ntru_sks, &api.ntru_sks));
  TEST_ASSERT_TRUE(
      mam_ntru_pk_t_set_cmp(&deserialized_api.ntru_pks, &api.ntru_pks));
  TEST_ASSERT_TRUE(mam_psk_t_set_cmp(&deserialized_api.psks, &api.psks));
  TEST_ASSERT_TRUE(trit_t_to_mam_msg_write_context_t_map_cmp(
      &deserialized_api.write_ctxs, &api.write_ctxs));
  TEST_ASSERT_TRUE(trit_t_to_mam_msg_read_context_t_map_cmp(
      &deserialized_api.read_ctxs, &api.read_ctxs));
  TEST_ASSERT_TRUE(
      mam_channel_t_set_cmp(&deserialized_api.channels, &api.channels));

  mam_api_destroy(&deserialized_api);
}

static void test_api_save_load() {
  mam_api_t loaded_api;

  TEST_ASSERT(mam_api_save(&api, "mam-api.bin") == RC_OK);
  TEST_ASSERT(mam_api_load("mam-api.bin", &loaded_api) == RC_OK);

  TEST_ASSERT_EQUAL_MEMORY(&loaded_api.prng, &api.prng, MAM_PRNG_KEY_SIZE);
  TEST_ASSERT_TRUE(mam_ntru_sk_t_set_cmp(&loaded_api.ntru_sks, &api.ntru_sks));
  TEST_ASSERT_TRUE(mam_ntru_pk_t_set_cmp(&loaded_api.ntru_pks, &api.ntru_pks));
  TEST_ASSERT_TRUE(mam_psk_t_set_cmp(&loaded_api.psks, &api.psks));
  TEST_ASSERT_TRUE(trit_t_to_mam_msg_write_context_t_map_cmp(
      &loaded_api.write_ctxs, &api.write_ctxs));
  TEST_ASSERT_TRUE(trit_t_to_mam_msg_read_context_t_map_cmp(
      &loaded_api.read_ctxs, &api.read_ctxs));
  TEST_ASSERT_TRUE(mam_channel_t_set_cmp(&loaded_api.channels, &api.channels));

  mam_api_destroy(&loaded_api);
}

int main(void) {
  UNITY_BEGIN();

  TEST_ASSERT(mam_api_init(&api, API_SEED) == RC_OK);
  test_api_create_channels(&api, &cha, &ch1a, &epa, &ep1a);

  RUN_TEST(test_api_generic);
  RUN_TEST(test_api_multiple_packets);
  RUN_TEST(test_api_serialization);
  RUN_TEST(test_api_save_load);

  /* destroy channels/endpoints */
  {
    if (cha) {
      mam_channel_destroy(cha);
      free(cha);
    }
    if (ch1a) {
      mam_channel_destroy(ch1a);
      free(ch1a);
    }
    if (epa) {
      mam_endpoint_destroy(epa);
      free(epa);
    }
    if (ep1a) {
      mam_endpoint_destroy(ep1a);
      free(ep1a);
    }
  }
  TEST_ASSERT(mam_api_destroy(&api) == RC_OK);

  return UNITY_END();
}
