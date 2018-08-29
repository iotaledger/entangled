/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/uri_parser.h"
#include <stdio.h>
#include <unity/unity.h>

void test_uri_parser_ipv4(void) {
  char *uri_ipv4 = "tcp://8.8.8.8:4242";
  size_t scheme_len = 5;
  size_t host_len = 17;
  char scheme[scheme_len];
  char host[host_len];
  uint16_t port;

  TEST_ASSERT_TRUE(
      uri_parse(uri_ipv4, scheme, scheme_len, host, host_len, &port));
  TEST_ASSERT_EQUAL_STRING("tcp", scheme);
  TEST_ASSERT_EQUAL_STRING("8.8.8.8", host);
  TEST_ASSERT_EQUAL_INT(4242, port);
}

void test_uri_parser_ipv6(void) {
  char *uri_ipv6 = "tcp://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:4242";
  size_t scheme_len = 5;
  size_t host_len = 40;
  char scheme[scheme_len];
  char host[host_len];
  uint16_t port;

  TEST_ASSERT_TRUE(
      uri_parse(uri_ipv6, scheme, scheme_len, host, host_len, &port));
  TEST_ASSERT_EQUAL_STRING("tcp", scheme);
  TEST_ASSERT_EQUAL_STRING("FEDC:BA98:7654:3210:FEDC:BA98:7654:3210", host);
  TEST_ASSERT_EQUAL_INT(4242, port);
}

void test_uri_parser_http(void) {
  char *uri_http = "http://iota.tangle.ncku:4242";
  size_t scheme_len = 5;
  size_t host_len = 17;
  char scheme[scheme_len];
  char host[host_len];
  uint16_t port;

  TEST_ASSERT_TRUE(
      uri_parse(uri_http, scheme, scheme_len, host, host_len, &port));
  TEST_ASSERT_EQUAL_STRING("http", scheme);
  TEST_ASSERT_EQUAL_STRING("iota.tangle.ncku", host);
  TEST_ASSERT_EQUAL_INT(4242, port);

  char *uri_https = "https://iota.ncku:4242";
  TEST_ASSERT_TRUE(
      uri_parse(uri_https, scheme, scheme_len, host, host_len, &port));
  TEST_ASSERT_EQUAL_STRING("https", scheme);
  TEST_ASSERT_EQUAL_STRING("iota.ncku", host);
  TEST_ASSERT_EQUAL_INT(4242, port);
}
void test_wrong_uri(void) {
  char *ipv6_wrong_case1 = "tcp://FEDC:BA98:7654:3210:FEDC:BA98:7654:3210:4242";
  char *ipv6_wrong_case2 =
      "tcp://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]4242";
  char *ipv6_wrong_case3 =
      "ldm://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]4242";
  size_t scheme_len = 5;
  size_t host_len = 40;
  char scheme[scheme_len];
  char host[host_len];
  uint16_t port;

  TEST_ASSERT_FALSE(
      uri_parse(ipv6_wrong_case1, scheme, scheme_len, host, host_len, &port));
  TEST_ASSERT_FALSE(
      uri_parse(ipv6_wrong_case2, scheme, scheme_len, host, host_len, &port));
  TEST_ASSERT_FALSE(
      uri_parse(ipv6_wrong_case3, scheme, scheme_len, host, host_len, &port));
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_uri_parser_ipv4);
  RUN_TEST(test_uri_parser_ipv6);
  RUN_TEST(test_uri_parser_http);
  RUN_TEST(test_wrong_uri);

  return UNITY_END();
}