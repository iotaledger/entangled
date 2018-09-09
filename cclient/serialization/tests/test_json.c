/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "test_json.h"

void test_serialize_find_transactions(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"findTransactions\",\"addresses\":["
      "\"RVORZ9SIIP9RCYMREUIXXVPQIPHVCNPQ9HZWYKFWYWZRE9JQKG9REPKIASHUUECPSQO9JT"
      "9XNMVKWYGVAZETAIRPTM\"],\"approvees\":["
      "\"LCIKYSBE9IHXLIKCEJTTIQOTTAWSQCCQQ9A9VOKIWRBWVPXMCGUENWVVMQAMPEIVHEQ9JX"
      "LCNZOORVZTZ\"],\"bundles\":["
      "\"VUDHGOXOYEVRPUHAWOUEPHUXVTTUTDUJFFJNBFGWISXQSNRVBUHKETTMEPBXYXEPJAVVX9"
      "LKUSJNAECEW\"],\"tags\":[\"MINEIOTADOTCOM9999999999999\"]}";

  find_transactions_req_t* find_tran = find_transactions_req_new();
  char_buffer_t* serializer_out = char_buffer_new();

  find_transactions_req_add_tag(find_tran, "MINEIOTADOTCOM9999999999999");
  find_transactions_req_add_approvee(
      find_tran,
      "LCIKYSBE9IHXLIKCEJTTIQOTTAWSQCCQQ9A9VOKIWRBWVPXMCGUENWVVMQAMPEIVHEQ9JXLC"
      "NZOORVZTZ");
  find_transactions_req_add_address(
      find_tran,
      "RVORZ9SIIP9RCYMREUIXXVPQIPHVCNPQ9HZWYKFWYWZRE9JQKG9REPKIASHUUECPSQO9JT9X"
      "NMVKWYGVAZETAIRPTM");
  find_transactions_req_add_bundle(find_tran,
                                   "VUDHGOXOYEVRPUHAWOUEPHUXVTTUTDUJFFJNBFGWISX"
                                   "QSNRVBUHKETTMEPBXYXEPJAVVX9LKUSJNAECEW");

  serializer.vtable.find_transactions_serialize_request(&serializer, find_tran,
                                                        serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  find_transactions_req_free(&find_tran);
}

void test_deserialize_find_transactions(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"hashes\": "
      "[\"ZJVYUGTDRPDYFGFXMKOTV9ZWSGFK9CFPXTITQLQNLPPG9YNAARMKNKYQO9GSCSBIOTGML"
      "JUFLZWSY9999\", "
      "\"9CZ9KJZCDJIKWARATSNPHNWPUM9WHNTCQLZKYQBZIWMXIQXFFKMMHLXIBYXZBALCUVD9X9"
      "NTNVAGNWGUW\", "
      "\"ZJVYUGTDRPDYF9999KOTV9ZWSGFK9CFPXTITQLQNLPPG9YNAARMKNKYQO9GSCSBIOTGMLJ"
      "UFLZWSY9999\"]}";

  find_transactions_res_t* deserialize_find_tran = find_transactions_res_new();

  serializer.vtable.find_transactions_deserialize_response(
      &serializer, json_text, deserialize_find_tran);
  TEST_ASSERT_EQUAL_STRING(
      "ZJVYUGTDRPDYFGFXMKOTV9ZWSGFK9CFPXTITQLQNLPPG9YNAARMKNKYQO9GSCSBIOTGMLJUF"
      "LZWSY9999",
      find_transactions_res_hash_at(deserialize_find_tran, 0));
  TEST_ASSERT_EQUAL_STRING(
      "9CZ9KJZCDJIKWARATSNPHNWPUM9WHNTCQLZKYQBZIWMXIQXFFKMMHLXIBYXZBALCUVD9X9NT"
      "NVAGNWGUW",
      find_transactions_res_hash_at(deserialize_find_tran, 1));
  TEST_ASSERT_EQUAL_STRING(
      NULL, find_transactions_res_hash_at(deserialize_find_tran, 3));
  find_transactions_res_free(&deserialize_find_tran);
}

void test_serialize_get_node_info(void) {
  serializer_t serializer;
  const char* json_text = "{\"command\":\"getNodeInfo\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  serializer.vtable.get_node_info_serialize_request(&serializer,
                                                    serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

void test_deserialize_get_node_info(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"appName\":\"IRI\",\"appVersion\":\"1.0.8.nu\",\"duration\":1,"
      "\"jreAvailableProcessors\":4,\"jreFreeMemory\":91707424,"
      "\"jreMaxMemory\":1908932608,\"jreTotalMemory\":122683392,"
      "\"latestMilestone\":"
      "\"VBVEUQYE99LFWHDZRFKTGFHYGDFEAMAEBGUBTTJRFKHCFBRTXFAJQ9XIUEZQCJOQTZNOOH"
      "KUQIKOY9999\",\"latestMilestoneIndex\":107,"
      "\"latestSolidSubtangleMilestone\":"
      "\"VBVEUQYE99LFWHDZRFKTGFHYGDFEAMAEBGUBTTJRFKHCFBRTXFAJQ9XIUEZQCJOQTZNOOH"
      "KUQIKOY9999\",\"latestSolidSubtangleMilestoneIndex\":107,\"neighbors\":"
      "2,\"packetsQueueSize\":0,\"time\":1477037811737,\"tips\":3,"
      "\"transactionsToRequest\":0}";

  get_node_info_res_t* node_info = get_node_info_res_new();

  serializer.vtable.get_node_info_deserialize_response(&serializer, json_text,
                                                       node_info);

  TEST_ASSERT_EQUAL_STRING("IRI", node_info->app_name->data);
  TEST_ASSERT_EQUAL_STRING("1.0.8.nu", node_info->app_version->data);
  TEST_ASSERT_EQUAL_INT(4, node_info->jre_available_processors);
  TEST_ASSERT_EQUAL_INT8(91707424, node_info->jre_free_memory);
  TEST_ASSERT_EQUAL_INT8(1908932608, node_info->jre_max_memory);
  TEST_ASSERT_EQUAL_INT8(122683392, node_info->jre_total_memory);
  TEST_ASSERT_EQUAL_STRING(
      "VBVEUQYE99LFWHDZRFKTGFHYGDFEAMAEBGUBTTJRFKHCFBRTXFAJQ9XIUEZQCJOQTZNOOHKU"
      "QIKOY9999",
      node_info->latest_milestone->data);
  TEST_ASSERT_EQUAL_INT8(107, node_info->latest_milestone_index);
  TEST_ASSERT_EQUAL_STRING(
      "VBVEUQYE99LFWHDZRFKTGFHYGDFEAMAEBGUBTTJRFKHCFBRTXFAJQ9XIUEZQCJOQTZNOOHKU"
      "QIKOY9999",
      node_info->latest_solid_subtangle_milestone->data);
  TEST_ASSERT_EQUAL_INT8(107,
                         node_info->latest_solid_subtangle_milestone_index);
  TEST_ASSERT_EQUAL_INT(2, node_info->neighbors);
  TEST_ASSERT_EQUAL_INT(0, node_info->packets_queue_size);
  TEST_ASSERT_EQUAL_INT8(1477037811737, node_info->time);
  TEST_ASSERT_EQUAL_INT(3, node_info->tips);
  TEST_ASSERT_EQUAL_INT(0, node_info->trans_to_request);

  get_node_info_res_free(node_info);
}

void test_serialize_get_neighbors(void) {
  serializer_t serializer;
  const char* json_text = "{\"command\":\"getNeighbors\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  serializer.vtable.get_neighbors_serialize_request(&serializer,
                                                    serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

void test_deserialize_get_neighbors(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"duration\":37,\"neighbors\":[{\"address\":\"/"
      "8.8.8.8:14265\",\"numberOfAllTransactions\":922,"
      "\"numberOfInvalidTransactions\":0,\"numberOfNewTransactions\":92},{"
      "\"address\":\"/"
      "8.8.8.8:5000\",\"numberOfAllTransactions\":925,"
      "\"numberOfInvalidTransactions\":0,\"numberOfNewTransactions\":20}]}";

  get_neighbors_res_t* nbors = get_neighbors_res_new();

  serializer.vtable.get_neighbors_deserialize_response(&serializer, json_text,
                                                       nbors);

  neighbor_info_t* nb = get_neighbors_res_neighbor_at(nbors, 0);
  TEST_ASSERT_EQUAL_STRING("/8.8.8.8:14265", nb->address->data);
  TEST_ASSERT_EQUAL_INT(922, nb->all_trans_num);
  TEST_ASSERT_EQUAL_INT(0, nb->invalid_trans_num);
  TEST_ASSERT_EQUAL_INT(92, nb->new_trans_num);
  nb = get_neighbors_res_neighbor_at(nbors, 1);
  TEST_ASSERT_EQUAL_STRING("/8.8.8.8:5000", nb->address->data);
  TEST_ASSERT_EQUAL_INT(925, nb->all_trans_num);
  TEST_ASSERT_EQUAL_INT(0, nb->invalid_trans_num);
  TEST_ASSERT_EQUAL_INT(20, nb->new_trans_num);
  nb = get_neighbors_res_neighbor_at(nbors, 2);
  TEST_ASSERT_NULL(nb);
  get_neighbors_res_free(nbors);
}

void test_serialize_add_neighbors(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"addNeighbors\",\"uris\":[\"udp://8.8.8.8:14265\",\"udp://"
      "9.9.9.9:443\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  add_neighbors_req_t* req = add_neighbors_req_new();
  add_neighbors_req_add(req, "udp://8.8.8.8:14265");
  add_neighbors_req_add(req, "udp://9.9.9.9:443");

  serializer.vtable.add_neighbors_serialize_request(&serializer, req,
                                                    serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  add_neighbors_req_free(req);
}

void test_deserialize_add_neighbors(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text = "{\"addedNeighbors\":3,\"duration\":2}";

  add_neighbors_res_t res = 0;

  serializer.vtable.add_neighbors_deserialize_response(&serializer, json_text,
                                                       &res);

  TEST_ASSERT_EQUAL_INT(3, res);
}

void test_serialize_remove_neighbors(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"removeNeighbors\",\"uris\":[\"udp://"
      "8.8.8.8:14265\",\"udp://"
      "9.9.9.9:443\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  remove_neighbors_req_t* req = remove_neighbors_req_new();
  remove_neighbors_req_add(req, "udp://8.8.8.8:14265");
  remove_neighbors_req_add(req, "udp://9.9.9.9:443");

  serializer.vtable.remove_neighbors_serialize_request(&serializer, req,
                                                       serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  remove_neighbors_req_free(req);
}

void test_deserialize_remove_neighbors(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text = "{\"removedNeighbors\":1,\"duration\":2}";

  remove_neighbors_res_t res = 0;

  serializer.vtable.remove_neighbors_deserialize_response(&serializer,
                                                          json_text, &res);

  TEST_ASSERT_EQUAL_INT(1, res);
}

void test_serialize_get_tips(void) {
  serializer_t serializer;
  const char* json_text = "{\"command\":\"getTips\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  serializer.vtable.get_tips_serialize_request(&serializer, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

void test_deserialize_get_tips(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"hashes\":["
      "\"YVXJOEOP9JEPRQUVBPJMB9MGIB9OMTIJJLIUYPM9YBIWXPZ9PQCCGXYSLKQWKHBRVA9AKK"
      "KXXMXF99999\","
      "\"ZUMARCWKZOZRMJM9EEYJQCGXLHWXPRTMNWPBRCAGSGQNRHKGRUCIYQDAEUUEBRDBNBYHAQ"
      "SSFZZQW9999\","
      "\"QLQECHDVQBMXKD9YYLBMGQLLIQ9PSOVDRLYCLLFMS9O99XIKCUHWAFWSTARYNCPAVIQIBT"
      "VJROOYZ9999\"],\"duration\":4}";

  get_tips_res_t* tips = get_tips_res_new();

  serializer.vtable.get_tips_deserialize_response(&serializer, json_text, tips);
  TEST_ASSERT_EQUAL_STRING(
      "YVXJOEOP9JEPRQUVBPJMB9MGIB9OMTIJJLIUYPM9YBIWXPZ9PQCCGXYSLKQWKHBRVA9AKKKX"
      "XMXF99999",
      get_tips_res_hash_at(tips, 0));
  TEST_ASSERT_EQUAL_STRING(
      "ZUMARCWKZOZRMJM9EEYJQCGXLHWXPRTMNWPBRCAGSGQNRHKGRUCIYQDAEUUEBRDBNBYHAQSS"
      "FZZQW9999",
      get_tips_res_hash_at(tips, 1));
  TEST_ASSERT_EQUAL_STRING(NULL, get_tips_res_hash_at(tips, 3));
  get_tips_res_free(tips);
}

void test_serialize_get_trytes(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"getTrytes\",\"hashes\":["
      "\"OAATQS9VQLSXCLDJVJJVYUGONXAXOFMJOZNSYWRZSWECMXAQQURHQBJNLD9IOFEPGZEPEM"
      "PXCIVRX9999\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  get_trytes_req_t* req = get_trytes_req_new();
  get_trytes_req_add(req,
                     "OAATQS9VQLSXCLDJVJJVYUGONXAXOFMJOZNSYWRZSWECMXAQQURHQBJNL"
                     "D9IOFEPGZEPEMPXCIVRX9999");

  serializer.vtable.get_trytes_serialize_request(&serializer, req,
                                                 serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_trytes_req_free(req);
}

void test_deserialize_get_trytes(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"trytes\":["
      "\"BYSWEAUTWXHXZ9YBZISEK9LUHWGMHXCGEVNZHRLUWQFCUSDXZHOFHWHL9MQPVJXXZLIXPX"
      "PXF9KYEREFSKCPKYIIKPZVLHUTDFQKKVVBBN9ATTLPCNPJDWDEVIYYLGPZGCWXOBDXMLJC9V"
      "O9QXTTBLAXTTBFUAROYEGQIVB9MJWJKXJMCUPTWAUGFZBTZCSJVRBGMYXTVBDDS9MYUJCPZ9"
      "YDWWQNIPUAIJXXSNLKUBSCOIJPCLEFPOXFJREXQCUVUMKSDOVQGGHRNILCO9GNCLWFM9APMN"
      "MWYASHXQAYBEXF9QRIHIBHYEJOYHRQJAOKAQ9AJJFQ9WEIWIJOTZATIBOXQLBMIJU9PCGBLV"
      "DDVFP9CFFSXTDUXMEGOOFXWRTLFGV9XXMYWEMGQEEEDBTIJ9OJOXFAPFQXCDAXOUDMLVYRMR"
      "LUDBETOLRJQAEDDLNVIRQJUBZBO9CCFDHIX9MSQCWYAXJVWHCUPTRSXJDESISQPRKZAFKFRU"
      "LCGVRSBLVFOPEYLEE99JD9SEBALQINPDAZHFAB9RNBH9AZWIJOTLBZVIEJIAYGMC9AZGNFWG"
      "RSWAXTYSXVROVNKCOQQIWGPNQZKHUNODGYADPYLZZZUQRTJRTODOUKAOITNOMWNGHJBBA99Q"
      "UMBHRENGBHTH9KHUAOXBVIVDVYYZMSEYSJWIOGGXZVRGN999EEGQMCOYVJQRIRROMPCQBLDY"
      "IGQO9AMORPYFSSUGACOJXGAQSPDY9YWRRPESNXXBDQ9OZOXVIOMLGTSWAMKMTDRSPGJKGBXQ"
      "IVNRJRFRYEZ9VJDLHIKPSKMYC9YEGHFDS9SGVDHRIXBEMLFIINOHVPXIFAZCJKBHVMQZEVWC"
      "OSNWQRDYWVAIBLSCBGESJUIBWZECPUCAYAWMTQKRMCHONIPKJYYTEGZCJYCT9ABRWTJLRQXK"
      "MWY9GWZMHYZNWPXULNZAPVQLPMYQZCYNEPOCGOHBJUZLZDPIXVHLDMQYJUUBEDXXPXFLNRGI"
      "PWBRNQQZJSGSJTTYHIGGFAWJVXWL9THTPWOOHTNQWCNYOYZXALHAZXVMIZE9WMQUDCHDJMIB"
      "WKTYH9AC9AFOT9DPCADCV9ZWUTE9QNOMSZPTZDJLJZCJGHXUNBJFUBJWQUEZDMHXGBPTNSPZ"
      "BR9TGSKVOHMOQSWPGFLSWNESFKSAZY9HHERAXALZCABFYPOVLAHMIHVDBGKUMDXC9WHHTIRY"
      "HZVWNXSVQUWCR9M9RAGMFEZZKZ9XEOQGOSLFQCHHOKLDSA9QCMDGCGMRYJZLBVIFOLBIJPRO"
      "KMHOYTBTJIWUZWJMCTKCJKKTR9LCVYPVJI9AHGI9JOWMIWZAGMLDFJA9WU9QAMEFGABIBEZN"
      "NAL9OXSBFLOEHKDGHWFQSHMPLYFCNXAAZYJLMQDEYRGL9QKCEUEJ9LLVUOINVSZZQHCIKPAG"
      "MT9CAYIIMTTBCPKWTYHOJIIY9GYNPAJNUJ9BKYYXSV9JSPEXYMCFAIKTGNRSQGUNIYZCRT9F"
      "OWENSZQPD9ALUPYYAVICHVYELYFPUYDTWUSWNIYFXPX9MICCCOOZIWRNJIDALWGWRATGLJXN"
      "AYTNIZWQ9YTVDBOFZRKO9CFWRPAQQRXTPACOWCPRLYRYSJARRKSQPR9TCFXDVIXLP9XVL99E"
      "RRDSOHBFJDJQQGGGCZNDQ9NYCTQJWVZIAELCRBJJFDMCNZU9FIZRPGNURTXOCDSQGXTQHKHU"
      "ECGWFUUYS9J9NYQ9U9P9UUP9YMZHWWWCIASCFLCMSKTELZWUGCDE9YOKVOVKTAYPHDF9ZCCQ"
      "AYPJIJNGSHUIHHCOSSOOBUDOKE9CJZGYSSGNCQJVBEFTZFJ9SQUHOASKRRGBSHWKBCBWBTJH"
      "OGQ9WOMQFHWJVEG9NYX9KWBTCAIXNXHEBDIOFO9ALYMFGRICLCKKLG9FOBOX9PDWNQRGHBKH"
      "GKKRLWTBEQMCWQRLHAVYYZDIIPKVQTHYTWQMTOACXZOQCDTJTBAAUWXSGJF9PNQIJ9AJRUMU"
      "VCPWYVYVARKR9RKGOUHHNKNVGGPDDLGKPQNOYHNKAVVKCXWXOQPZNSLATUJT9AUWRMPPSWHS"
      "TTYDFAQDXOCYTZHOYYGAIM9CELMZ9AZPWB9MJXGHOKDNNSZVUDAGXTJJSSZCPZVPZBYNNTUQ"
      "ABSXQWZCHDQSLGK9UOHCFKBIBNETK9999999999999999999999999999999999999999999"
      "99999999999999999999999999999999999999NOXDXXKUDWLOFJLIPQIBRBMGDYCPGDNLQO"
      "LQS99EQYKBIU9VHCJVIPFUYCQDNY9APGEVYLCENJIOBLWNB999999999XKBRHUD99C999999"
      "99NKZKEKWLDKMJCI9N9XQOLWEPAYWSH9999999999999999999999999KDDTGZLIPBNZKMLT"
      "OLOXQVNGLASESDQVPTXALEKRMIOHQLUHD9ELQDBQETS9QFGTYOYWLNTSKKMVJAUXSIROUICD"
      "OXKSYZTDPEDKOQENTJOWJONDEWROCEJIEWFWLUAACVSJFTMCHHXJBJRKAAPUDXXVXFWP9X99"
      "99IROUICDOXKSYZTDPEDKOQENTJOWJONDEWROCEJIEWFWLUAACVSJFTMCHHXJBJRKAAPUDXX"
      "VXFWP9X9999\"]}";

  get_trytes_res_t* tips = get_trytes_res_new();

  serializer.vtable.get_trytes_deserialize_response(&serializer, json_text,
                                                    tips);
  TEST_ASSERT_EQUAL_STRING(
      "BYSWEAUTWXHXZ9YBZISEK9LUHWGMHXCGEVNZHRLUWQFCUSDXZHOFHWHL9MQPVJXXZLIXPXPX"
      "F9KYEREFSKCPKYIIKPZVLHUTDFQKKVVBBN9ATTLPCNPJDWDEVIYYLGPZGCWXOBDXMLJC9VO9"
      "QXTTBLAXTTBFUAROYEGQIVB9MJWJKXJMCUPTWAUGFZBTZCSJVRBGMYXTVBDDS9MYUJCPZ9YD"
      "WWQNIPUAIJXXSNLKUBSCOIJPCLEFPOXFJREXQCUVUMKSDOVQGGHRNILCO9GNCLWFM9APMNMW"
      "YASHXQAYBEXF9QRIHIBHYEJOYHRQJAOKAQ9AJJFQ9WEIWIJOTZATIBOXQLBMIJU9PCGBLVDD"
      "VFP9CFFSXTDUXMEGOOFXWRTLFGV9XXMYWEMGQEEEDBTIJ9OJOXFAPFQXCDAXOUDMLVYRMRLU"
      "DBETOLRJQAEDDLNVIRQJUBZBO9CCFDHIX9MSQCWYAXJVWHCUPTRSXJDESISQPRKZAFKFRULC"
      "GVRSBLVFOPEYLEE99JD9SEBALQINPDAZHFAB9RNBH9AZWIJOTLBZVIEJIAYGMC9AZGNFWGRS"
      "WAXTYSXVROVNKCOQQIWGPNQZKHUNODGYADPYLZZZUQRTJRTODOUKAOITNOMWNGHJBBA99QUM"
      "BHRENGBHTH9KHUAOXBVIVDVYYZMSEYSJWIOGGXZVRGN999EEGQMCOYVJQRIRROMPCQBLDYIG"
      "QO9AMORPYFSSUGACOJXGAQSPDY9YWRRPESNXXBDQ9OZOXVIOMLGTSWAMKMTDRSPGJKGBXQIV"
      "NRJRFRYEZ9VJDLHIKPSKMYC9YEGHFDS9SGVDHRIXBEMLFIINOHVPXIFAZCJKBHVMQZEVWCOS"
      "NWQRDYWVAIBLSCBGESJUIBWZECPUCAYAWMTQKRMCHONIPKJYYTEGZCJYCT9ABRWTJLRQXKMW"
      "Y9GWZMHYZNWPXULNZAPVQLPMYQZCYNEPOCGOHBJUZLZDPIXVHLDMQYJUUBEDXXPXFLNRGIPW"
      "BRNQQZJSGSJTTYHIGGFAWJVXWL9THTPWOOHTNQWCNYOYZXALHAZXVMIZE9WMQUDCHDJMIBWK"
      "TYH9AC9AFOT9DPCADCV9ZWUTE9QNOMSZPTZDJLJZCJGHXUNBJFUBJWQUEZDMHXGBPTNSPZBR"
      "9TGSKVOHMOQSWPGFLSWNESFKSAZY9HHERAXALZCABFYPOVLAHMIHVDBGKUMDXC9WHHTIRYHZ"
      "VWNXSVQUWCR9M9RAGMFEZZKZ9XEOQGOSLFQCHHOKLDSA9QCMDGCGMRYJZLBVIFOLBIJPROKM"
      "HOYTBTJIWUZWJMCTKCJKKTR9LCVYPVJI9AHGI9JOWMIWZAGMLDFJA9WU9QAMEFGABIBEZNNA"
      "L9OXSBFLOEHKDGHWFQSHMPLYFCNXAAZYJLMQDEYRGL9QKCEUEJ9LLVUOINVSZZQHCIKPAGMT"
      "9CAYIIMTTBCPKWTYHOJIIY9GYNPAJNUJ9BKYYXSV9JSPEXYMCFAIKTGNRSQGUNIYZCRT9FOW"
      "ENSZQPD9ALUPYYAVICHVYELYFPUYDTWUSWNIYFXPX9MICCCOOZIWRNJIDALWGWRATGLJXNAY"
      "TNIZWQ9YTVDBOFZRKO9CFWRPAQQRXTPACOWCPRLYRYSJARRKSQPR9TCFXDVIXLP9XVL99ERR"
      "DSOHBFJDJQQGGGCZNDQ9NYCTQJWVZIAELCRBJJFDMCNZU9FIZRPGNURTXOCDSQGXTQHKHUEC"
      "GWFUUYS9J9NYQ9U9P9UUP9YMZHWWWCIASCFLCMSKTELZWUGCDE9YOKVOVKTAYPHDF9ZCCQAY"
      "PJIJNGSHUIHHCOSSOOBUDOKE9CJZGYSSGNCQJVBEFTZFJ9SQUHOASKRRGBSHWKBCBWBTJHOG"
      "Q9WOMQFHWJVEG9NYX9KWBTCAIXNXHEBDIOFO9ALYMFGRICLCKKLG9FOBOX9PDWNQRGHBKHGK"
      "KRLWTBEQMCWQRLHAVYYZDIIPKVQTHYTWQMTOACXZOQCDTJTBAAUWXSGJF9PNQIJ9AJRUMUVC"
      "PWYVYVARKR9RKGOUHHNKNVGGPDDLGKPQNOYHNKAVVKCXWXOQPZNSLATUJT9AUWRMPPSWHSTT"
      "YDFAQDXOCYTZHOYYGAIM9CELMZ9AZPWB9MJXGHOKDNNSZVUDAGXTJJSSZCPZVPZBYNNTUQAB"
      "SXQWZCHDQSLGK9UOHCFKBIBNETK999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999NOXDXXKUDWLOFJLIPQIBRBMGDYCPGDNLQOLQ"
      "S99EQYKBIU9VHCJVIPFUYCQDNY9APGEVYLCENJIOBLWNB999999999XKBRHUD99C99999999"
      "NKZKEKWLDKMJCI9N9XQOLWEPAYWSH9999999999999999999999999KDDTGZLIPBNZKMLTOL"
      "OXQVNGLASESDQVPTXALEKRMIOHQLUHD9ELQDBQETS9QFGTYOYWLNTSKKMVJAUXSIROUICDOX"
      "KSYZTDPEDKOQENTJOWJONDEWROCEJIEWFWLUAACVSJFTMCHHXJBJRKAAPUDXXVXFWP9X9999"
      "IROUICDOXKSYZTDPEDKOQENTJOWJONDEWROCEJIEWFWLUAACVSJFTMCHHXJBJRKAAPUDXXVX"
      "FWP9X9999",
      get_trytes_res_at(tips, 0));
  TEST_ASSERT_EQUAL_STRING(NULL, get_trytes_res_at(tips, 3));
  get_trytes_res_free(tips);
}

void test_serialize_get_inclusion_states(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getInclusionStates\",\"transactions\":["
      "\"" INCLUSION_STATES_HASH "\"],\"tips\":[\"" INCLUSION_STATES_TIPS
      "\"]}";

  get_inclusion_state_req_t* get_is = get_inclusion_state_req_new();
  char_buffer_t* serializer_out = char_buffer_new();

  get_inclusion_state_req_add_hash(get_is, INCLUSION_STATES_HASH);
  get_inclusion_state_req_add_tip(get_is, INCLUSION_STATES_TIPS);

  serializer.vtable.get_inclusion_state_serialize_request(&serializer, get_is,
                                                          serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_inclusion_state_req_free(&get_is);
}

void test_deserialize_get_inclusion_states(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text = "{\"states\": [true,false]}";
  get_inclusion_state_res_t* deserialize_get_is = get_inclusion_state_res_new();

  serializer.vtable.get_inclusion_state_deserialize_response(
      &serializer, json_text, deserialize_get_is);
  TEST_ASSERT_TRUE(get_inclusion_state_res_bool_at(deserialize_get_is, 0) ==
                   true);
  TEST_ASSERT_TRUE(get_inclusion_state_res_bool_at(deserialize_get_is, 1) ==
                   false);
  TEST_ASSERT_TRUE(get_inclusion_state_res_bool_at(deserialize_get_is, 2) ==
                   false);
  TEST_ASSERT_FALSE(get_inclusion_state_res_bool_at(deserialize_get_is, 2) ==
                    true);
  get_inclusion_state_res_free(&deserialize_get_is);
}

void test_serialize_get_balances(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getBalances\",\"addresses\":["
      "\"" GET_BALANCES_SERIALIZE_ADDRESS
      "\"]"
      ",\"threshold\":100}";

  char_buffer_t* serializer_out = char_buffer_new();
  get_balances_req_t* get_bal = get_balances_req_new();
  get_balances_req_add_address(get_bal,
                               "HBBYKAKTILIPVUKFOTSLHGENPTXYBNKXZFQFR9"
                               "VQFWNBMTQNRVOUKPVPRNBSZVVILMAFBKOTBLGL"
                               "WLOHQ");
  get_bal->threshold = 100;
  serializer.vtable.get_balances_serialize_request(&serializer, get_bal,
                                                   serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_balances_req_free(get_bal);
}

void test_deserialize_get_balances(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"balances\": "
      "[\"" GET_BALANCES_DESERIALIZE_BALANCE
      "\"], "
      "\"references\": "
      "[\"" GET_BALANCES_DESERIALIZE_REFERENCE
      "\"], "
      "\"milestoneIndex\":" GET_BALANCES_DESERIALIZE_MILESTONEINDEX "}";

  get_balances_res_t* deserialize_get_bal = get_balances_res_new();
  serializer.vtable.get_balances_deserialize_response(&serializer, json_text,
                                                      deserialize_get_bal);

  TEST_ASSERT_EQUAL_STRING(
      GET_BALANCES_DESERIALIZE_BALANCE,
      get_balances_res_balances_at(deserialize_get_bal, 0));
  TEST_ASSERT_EQUAL_STRING(
      GET_BALANCES_DESERIALIZE_REFERENCE,
      get_balances_res_milestone_at(deserialize_get_bal, 0));
  TEST_ASSERT_EQUAL_INT(atoi(GET_BALANCES_DESERIALIZE_MILESTONEINDEX),
                        deserialize_get_bal->milestoneIndex);
}

void test_serialize_get_transactions_to_approve(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getTransactionsToApprove\",\"depth\":" STR(GET_TRANSACTION_TO_APPROVE_DEPTH) 
      ",\"reference\":\"" GET_TRANSACTION_TO_APPROVE_HASH "\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  get_transactions_to_approve_req_t* get_txn_approve =
      get_transactions_to_approve_req_new();
  get_transactions_to_approve_req_set_reference(
      get_txn_approve, GET_TRANSACTION_TO_APPROVE_HASH);
  get_txn_approve->depth = GET_TRANSACTION_TO_APPROVE_DEPTH;
  serializer.vtable.get_transactions_to_approve_serialize_request(
      &serializer, get_txn_approve, serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_transactions_to_approve_req_free(&get_txn_approve);
}

void test_deserialize_get_transactions_to_approve(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"trunkTransaction\": "
      "\"" GET_TRANSACTION_TO_APPROVE_HASH
      "\",\"branchTransaction\":"
      "\"" GET_TRANSACTION_TO_APPROVE_HASH "\"}";

  get_transactions_to_approve_res_t* deserialize_get_txn_approve =
      get_transactions_to_approve_res_new();
  serializer.vtable.get_transactions_to_approve_deserialize_response(
      &serializer, json_text, deserialize_get_txn_approve);

  TEST_ASSERT_EQUAL_STRING(GET_TRANSACTION_TO_APPROVE_HASH,
                           deserialize_get_txn_approve->trunk->data);
  TEST_ASSERT_EQUAL_STRING(GET_TRANSACTION_TO_APPROVE_HASH,
                           deserialize_get_txn_approve->branch->data);

  get_transactions_to_approve_res_free(&deserialize_get_txn_approve);
}

void test_serialize_attach_to_tangle(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"attachToTangle\",\"trunkTransaction\":\"" TEST_HASH1
      "\",\"branchTransaction\":\"" TEST_HASH2
      "\",\"minWeightMagnitude\":18,\"trytes\":[\"" TEST_RAW_TRYTES1
      "\",\"" TEST_RAW_TRYTES2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  attach_to_tangle_req_t* attach_req = attach_to_tangle_req_new();

  attach_to_tangle_req_set_trunk(attach_req, TEST_HASH1);
  attach_to_tangle_req_set_branch(attach_req, TEST_HASH2);
  attach_to_tangle_req_add_trytes(attach_req, TEST_RAW_TRYTES1);
  attach_to_tangle_req_add_trytes(attach_req, TEST_RAW_TRYTES2);
  attach_to_tangle_req_set_mwm(attach_req, 18);
  serializer.vtable.attach_to_tangle_serialize_request(&serializer, attach_req,
                                                       serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  attach_to_tangle_req_free(&attach_req);
}

void test_deserialize_attach_to_tangle(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"trytes\":[\"" TEST_HASH1 "\",\"" TEST_HASH2 "\"],\"duration\":4}";

  attach_to_tangle_res_t* trytes = attach_to_tangle_res_new();

  serializer.vtable.attach_to_tangle_deserialize_response(&serializer,
                                                          json_text, trytes);
  TEST_ASSERT_EQUAL_STRING(TEST_HASH1,
                           attach_to_tangle_res_trytes_at(trytes, 0));
  TEST_ASSERT_EQUAL_STRING(TEST_HASH2,
                           attach_to_tangle_res_trytes_at(trytes, 1));
  TEST_ASSERT_EQUAL_STRING(NULL, attach_to_tangle_res_trytes_at(trytes, 3));
  attach_to_tangle_res_free(trytes);
}

void test_serialize_broadcast_transactions(void) {
  // TODO
}

void test_serialize_store_transactions(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"storeTransactions\",\"trytes\":["
      "\"BYSWEAUTWXHXZ9YBZISEK9LUHWGMHXCGEVNZHRLUWQFCUSDXZHOFHWHL9MQPVJXXZLIXPX"
      "PXF9KYEREFSKCPKYIIKPZVLHUTDFQKKVVBBN9ATTLPCNPJDWDEVIYYLGPZGCWXOBDXMLJC9V"
      "O9QXTTBLAXTTBFUAROYEGQIVB9MJWJKXJMCUPTWAUGFZBTZCSJVRBGMYXTVBDDS9MYUJCPZ9"
      "YDWWQNIPUAIJXXSNLKUBSCOIJPCLEFPOXFJREXQCUVUMKSDOVQGGHRNILCO9GNCLWFM9APMN"
      "MWYASHXQAYBEXF9QRIHIBHYEJOYHRQJAOKAQ9AJJFQ9WEIWIJOTZATIBOXQLBMIJU9PCGBLV"
      "DDVFP9CFFSXTDUXMEGOOFXWRTLFGV9XXMYWEMGQEEEDBTIJ9OJOXFAPFQXCDAXOUDMLVYRMR"
      "LUDBETOLRJQAEDDLNVIRQJUBZBO9CCFDHIX9MSQCWYAXJVWHCUPTRSXJDESISQPRKZAFKFRU"
      "LCGVRSBLVFOPEYLEE99JD9SEBALQINPDAZHFAB9RNBH9AZWIJOTLBZVIEJIAYGMC9AZGNFWG"
      "RSWAXTYSXVROVNKCOQQIWGPNQZKHUNODGYADPYLZZZUQRTJRTODOUKAOITNOMWNGHJBBA99Q"
      "UMBHRENGBHTH9KHUAOXBVIVDVYYZMSEYSJWIOGGXZVRGN999EEGQMCOYVJQRIRROMPCQBLDY"
      "IGQO9AMORPYFSSUGACOJXGAQSPDY9YWRRPESNXXBDQ9OZOXVIOMLGTSWAMKMTDRSPGJKGBXQ"
      "IVNRJRFRYEZ9VJDLHIKPSKMYC9YEGHFDS9SGVDHRIXBEMLFIINOHVPXIFAZCJKBHVMQZEVWC"
      "OSNWQRDYWVAIBLSCBGESJUIBWZECPUCAYAWMTQKRMCHONIPKJYYTEGZCJYCT9ABRWTJLRQXK"
      "MWY9GWZMHYZNWPXULNZAPVQLPMYQZCYNEPOCGOHBJUZLZDPIXVHLDMQYJUUBEDXXPXFLNRGI"
      "PWBRNQQZJSGSJTTYHIGGFAWJVXWL9THTPWOOHTNQWCNYOYZXALHAZXVMIZE9WMQUDCHDJMIB"
      "WKTYH9AC9AFOT9DPCADCV9ZWUTE9QNOMSZPTZDJLJZCJGHXUNBJFUBJWQUEZDMHXGBPTNSPZ"
      "BR9TGSKVOHMOQSWPGFLSWNESFKSAZY9HHERAXALZCABFYPOVLAHMIHVDBGKUMDXC9WHHTIRY"
      "HZVWNXSVQUWCR9M9RAGMFEZZKZ9XEOQGOSLFQCHHOKLDSA9QCMDGCGMRYJZLBVIFOLBIJPRO"
      "KMHOYTBTJIWUZWJMCTKCJKKTR9LCVYPVJI9AHGI9JOWMIWZAGMLDFJA9WU9QAMEFGABIBEZN"
      "NAL9OXSBFLOEHKDGHWFQSHMPLYFCNXAAZYJLMQDEYRGL9QKCEUEJ9LLVUOINVSZZQHCIKPAG"
      "MT9CAYIIMTTBCPKWTYHOJIIY9GYNPAJNUJ9BKYYXSV9JSPEXYMCFAIKTGNRSQGUNIYZCRT9F"
      "OWENSZQPD9ALUPYYAVICHVYELYFPUYDTWUSWNIYFXPX9MICCCOOZIWRNJIDALWGWRATGLJXN"
      "AYTNIZWQ9YTVDBOFZRKO9CFWRPAQQRXTPACOWCPRLYRYSJARRKSQPR9TCFXDVIXLP9XVL99E"
      "RRDSOHBFJDJQQGGGCZNDQ9NYCTQJWVZIAELCRBJJFDMCNZU9FIZRPGNURTXOCDSQGXTQHKHU"
      "ECGWFUUYS9J9NYQ9U9P9UUP9YMZHWWWCIASCFLCMSKTELZWUGCDE9YOKVOVKTAYPHDF9ZCCQ"
      "AYPJIJNGSHUIHHCOSSOOBUDOKE9CJZGYSSGNCQJVBEFTZFJ9SQUHOASKRRGBSHWKBCBWBTJH"
      "OGQ9WOMQFHWJVEG9NYX9KWBTCAIXNXHEBDIOFO9ALYMFGRICLCKKLG9FOBOX9PDWNQRGHBKH"
      "GKKRLWTBEQMCWQRLHAVYYZDIIPKVQTHYTWQMTOACXZOQCDTJTBAAUWXSGJF9PNQIJ9AJRUMU"
      "VCPWYVYVARKR9RKGOUHHNKNVGGPDDLGKPQNOYHNKAVVKCXWXOQPZNSLATUJT9AUWRMPPSWHS"
      "TTYDFAQDXOCYTZHOYYGAIM9CELMZ9AZPWB9MJXGHOKDNNSZVUDAGXTJJSSZCPZVPZBYNNTUQ"
      "ABSXQWZCHDQSLGK9UOHCFKBIBNETK9999999999999999999999999999999999999999999"
      "99999999999999999999999999999999999999NOXDXXKUDWLOFJLIPQIBRBMGDYCPGDNLQO"
      "LQS99EQYKBIU9VHCJVIPFUYCQDNY9APGEVYLCENJIOBLWNB999999999XKBRHUD99C999999"
      "99NKZKEKWLDKMJCI9N9XQOLWEPAYWSH9999999999999999999999999KDDTGZLIPBNZKMLT"
      "OLOXQVNGLASESDQVPTXALEKRMIOHQLUHD9ELQDBQETS9QFGTYOYWLNTSKKMVJAUXSIROUICD"
      "OXKSYZTDPEDKOQENTJOWJONDEWROCEJIEWFWLUAACVSJFTMCHHXJBJRKAAPUDXXVXFWP9X99"
      "99IROUICDOXKSYZTDPEDKOQENTJOWJONDEWROCEJIEWFWLUAACVSJFTMCHHXJBJRKAAPUDXX"
      "VXFWP9X9999\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  store_transactions_req_t* req = store_transactions_req_new();
  store_transactions_req_add(
      req,
      "BYSWEAUTWXHXZ9YBZISEK9LUHWGMHXCGEVNZHRLUWQFCUSDXZHOFHWHL9MQPVJXXZLIXPXPX"
      "F9KYEREFSKCPKYIIKPZVLHUTDFQKKVVBBN9ATTLPCNPJDWDEVIYYLGPZGCWXOBDXMLJC9VO9"
      "QXTTBLAXTTBFUAROYEGQIVB9MJWJKXJMCUPTWAUGFZBTZCSJVRBGMYXTVBDDS9MYUJCPZ9YD"
      "WWQNIPUAIJXXSNLKUBSCOIJPCLEFPOXFJREXQCUVUMKSDOVQGGHRNILCO9GNCLWFM9APMNMW"
      "YASHXQAYBEXF9QRIHIBHYEJOYHRQJAOKAQ9AJJFQ9WEIWIJOTZATIBOXQLBMIJU9PCGBLVDD"
      "VFP9CFFSXTDUXMEGOOFXWRTLFGV9XXMYWEMGQEEEDBTIJ9OJOXFAPFQXCDAXOUDMLVYRMRLU"
      "DBETOLRJQAEDDLNVIRQJUBZBO9CCFDHIX9MSQCWYAXJVWHCUPTRSXJDESISQPRKZAFKFRULC"
      "GVRSBLVFOPEYLEE99JD9SEBALQINPDAZHFAB9RNBH9AZWIJOTLBZVIEJIAYGMC9AZGNFWGRS"
      "WAXTYSXVROVNKCOQQIWGPNQZKHUNODGYADPYLZZZUQRTJRTODOUKAOITNOMWNGHJBBA99QUM"
      "BHRENGBHTH9KHUAOXBVIVDVYYZMSEYSJWIOGGXZVRGN999EEGQMCOYVJQRIRROMPCQBLDYIG"
      "QO9AMORPYFSSUGACOJXGAQSPDY9YWRRPESNXXBDQ9OZOXVIOMLGTSWAMKMTDRSPGJKGBXQIV"
      "NRJRFRYEZ9VJDLHIKPSKMYC9YEGHFDS9SGVDHRIXBEMLFIINOHVPXIFAZCJKBHVMQZEVWCOS"
      "NWQRDYWVAIBLSCBGESJUIBWZECPUCAYAWMTQKRMCHONIPKJYYTEGZCJYCT9ABRWTJLRQXKMW"
      "Y9GWZMHYZNWPXULNZAPVQLPMYQZCYNEPOCGOHBJUZLZDPIXVHLDMQYJUUBEDXXPXFLNRGIPW"
      "BRNQQZJSGSJTTYHIGGFAWJVXWL9THTPWOOHTNQWCNYOYZXALHAZXVMIZE9WMQUDCHDJMIBWK"
      "TYH9AC9AFOT9DPCADCV9ZWUTE9QNOMSZPTZDJLJZCJGHXUNBJFUBJWQUEZDMHXGBPTNSPZBR"
      "9TGSKVOHMOQSWPGFLSWNESFKSAZY9HHERAXALZCABFYPOVLAHMIHVDBGKUMDXC9WHHTIRYHZ"
      "VWNXSVQUWCR9M9RAGMFEZZKZ9XEOQGOSLFQCHHOKLDSA9QCMDGCGMRYJZLBVIFOLBIJPROKM"
      "HOYTBTJIWUZWJMCTKCJKKTR9LCVYPVJI9AHGI9JOWMIWZAGMLDFJA9WU9QAMEFGABIBEZNNA"
      "L9OXSBFLOEHKDGHWFQSHMPLYFCNXAAZYJLMQDEYRGL9QKCEUEJ9LLVUOINVSZZQHCIKPAGMT"
      "9CAYIIMTTBCPKWTYHOJIIY9GYNPAJNUJ9BKYYXSV9JSPEXYMCFAIKTGNRSQGUNIYZCRT9FOW"
      "ENSZQPD9ALUPYYAVICHVYELYFPUYDTWUSWNIYFXPX9MICCCOOZIWRNJIDALWGWRATGLJXNAY"
      "TNIZWQ9YTVDBOFZRKO9CFWRPAQQRXTPACOWCPRLYRYSJARRKSQPR9TCFXDVIXLP9XVL99ERR"
      "DSOHBFJDJQQGGGCZNDQ9NYCTQJWVZIAELCRBJJFDMCNZU9FIZRPGNURTXOCDSQGXTQHKHUEC"
      "GWFUUYS9J9NYQ9U9P9UUP9YMZHWWWCIASCFLCMSKTELZWUGCDE9YOKVOVKTAYPHDF9ZCCQAY"
      "PJIJNGSHUIHHCOSSOOBUDOKE9CJZGYSSGNCQJVBEFTZFJ9SQUHOASKRRGBSHWKBCBWBTJHOG"
      "Q9WOMQFHWJVEG9NYX9KWBTCAIXNXHEBDIOFO9ALYMFGRICLCKKLG9FOBOX9PDWNQRGHBKHGK"
      "KRLWTBEQMCWQRLHAVYYZDIIPKVQTHYTWQMTOACXZOQCDTJTBAAUWXSGJF9PNQIJ9AJRUMUVC"
      "PWYVYVARKR9RKGOUHHNKNVGGPDDLGKPQNOYHNKAVVKCXWXOQPZNSLATUJT9AUWRMPPSWHSTT"
      "YDFAQDXOCYTZHOYYGAIM9CELMZ9AZPWB9MJXGHOKDNNSZVUDAGXTJJSSZCPZVPZBYNNTUQAB"
      "SXQWZCHDQSLGK9UOHCFKBIBNETK999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999NOXDXXKUDWLOFJLIPQIBRBMGDYCPGDNLQOLQ"
      "S99EQYKBIU9VHCJVIPFUYCQDNY9APGEVYLCENJIOBLWNB999999999XKBRHUD99C99999999"
      "NKZKEKWLDKMJCI9N9XQOLWEPAYWSH9999999999999999999999999KDDTGZLIPBNZKMLTOL"
      "OXQVNGLASESDQVPTXALEKRMIOHQLUHD9ELQDBQETS9QFGTYOYWLNTSKKMVJAUXSIROUICDOX"
      "KSYZTDPEDKOQENTJOWJONDEWROCEJIEWFWLUAACVSJFTMCHHXJBJRKAAPUDXXVXFWP9X9999"
      "IROUICDOXKSYZTDPEDKOQENTJOWJONDEWROCEJIEWFWLUAACVSJFTMCHHXJBJRKAAPUDXXVX"
      "FWP9X9999");

  serializer.vtable.store_transactions_serialize_request(&serializer, req,
                                                         serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  store_transactions_req_free(req);
}

void test_serialize_were_addresses_spent_from(void) {
  serializer_t serializer;
  const char* json_text =
      "{"
      "\"command\":\"wereAddressesSpentFrom\","
      "\"trytes\":[\"" WERE_ADDRESSES_SPENT_FROM_TRYTES "\"]"
      "}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  were_addresses_spent_from_req_t* req = were_addresses_spent_from_req_new();
  were_addresses_spent_from_req_add(req, WERE_ADDRESSES_SPENT_FROM_TRYTES);

  serializer.vtable.were_addresses_spent_from_serialize_request(
      &serializer, req, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

void test_deserialize_were_addresses_spent_from(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{"
      "\"states\":[\"true\"]"
      "}";

  were_addresses_spent_from_res_t* res = were_addresses_spent_from_res_new();
  serializer.vtable.were_addresses_spent_from_deserialize_response(
      &serializer, json_text, res);

  TEST_ASSERT_EQUAL_STRING("true", were_addresses_spent_from_res_at(res, 0));
}

int main(void) {
  UNITY_BEGIN();

  // find_transactions
  RUN_TEST(test_serialize_find_transactions);
  RUN_TEST(test_deserialize_find_transactions);

  // get_node_info
  RUN_TEST(test_serialize_get_node_info);
  RUN_TEST(test_deserialize_get_node_info);

  // get_neighbors
  RUN_TEST(test_serialize_get_neighbors);
  RUN_TEST(test_deserialize_get_neighbors);

  // add_neighbors
  RUN_TEST(test_serialize_add_neighbors);
  RUN_TEST(test_deserialize_add_neighbors);

  // remove_neighbors
  RUN_TEST(test_serialize_remove_neighbors);
  RUN_TEST(test_deserialize_remove_neighbors);

  RUN_TEST(test_serialize_get_tips);
  RUN_TEST(test_deserialize_get_tips);

  RUN_TEST(test_serialize_get_trytes);
  RUN_TEST(test_deserialize_get_trytes);

  RUN_TEST(test_serialize_get_inclusion_states);
  RUN_TEST(test_deserialize_get_inclusion_states);

  RUN_TEST(test_serialize_get_balances);
  RUN_TEST(test_deserialize_get_balances);

  RUN_TEST(test_serialize_get_transactions_to_approve);
  RUN_TEST(test_deserialize_get_transactions_to_approve);

  RUN_TEST(test_serialize_attach_to_tangle);
  RUN_TEST(test_deserialize_attach_to_tangle);

  RUN_TEST(test_serialize_broadcast_transactions);

  RUN_TEST(test_serialize_store_transactions);

  RUN_TEST(test_serialize_were_addresses_spent_from);
  RUN_TEST(test_deserialize_were_addresses_spent_from);
  return UNITY_END();
}
