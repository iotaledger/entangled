/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/uri_parser.h"
#include <stdio.h>
#include <string.h>
#include <unity/unity.h>

void test_uri_parser_ipv4(void) {
  char *uri_ipv4 = PROTOCOL_TCP "://" HOST_IPv4 ":" PORT;
  size_t scheme_len = 5;
  size_t host_len = 17;
  char scheme[scheme_len];
  char host[host_len];
  uint16_t port;

  TEST_ASSERT_TRUE(
      uri_parse(uri_ipv4, scheme, scheme_len, host, host_len, &port));
  TEST_ASSERT_EQUAL_STRING(PROTOCOL_TCP, scheme);
  TEST_ASSERT_EQUAL_STRING(HOST_IPv4, host);
  TEST_ASSERT_EQUAL_INT(atoi(PORT), port);
}

void test_uri_parser_ipv6(void) {
  char *uri_ipv6 = PROTOCOL_TCP "://[" HOST_IPv6 "]:" PORT;
  size_t scheme_len = 5;
  size_t host_len = 40;
  char scheme[scheme_len];
  char host[host_len];
  uint16_t port;

  TEST_ASSERT_TRUE(
      uri_parse(uri_ipv6, scheme, scheme_len, host, host_len, &port));
  TEST_ASSERT_EQUAL_STRING(PROTOCOL_TCP, scheme);
  TEST_ASSERT_EQUAL_STRING(HOST_IPv6, host);
  TEST_ASSERT_EQUAL_INT(atoi(PORT), port);
}

void test_uri_parser_http(void) {
  char *uri_http = PROTOCOL_HTTP "://" HOST_HTTP ":" PORT;
  size_t scheme_len = 6;
  size_t host_len = strlen(HOST_HTTP) + 1;
  char scheme[scheme_len];
  char host[host_len];
  uint16_t port;

  TEST_ASSERT_TRUE(
      uri_parse(uri_http, scheme, scheme_len, host, host_len, &port));
  TEST_ASSERT_EQUAL_STRING(PROTOCOL_HTTP, scheme);
  TEST_ASSERT_EQUAL_STRING(HOST_HTTP, host);
  TEST_ASSERT_EQUAL_INT(atoi(PORT), port);

  char *uri_https = PROTOCOL_HTTPS "://" HOST_HTTP ":" PORT;
  TEST_ASSERT_TRUE(
      uri_parse(uri_https, scheme, scheme_len, host, host_len, &port));
  TEST_ASSERT_EQUAL_STRING(PROTOCOL_HTTPS, scheme);
  TEST_ASSERT_EQUAL_STRING(HOST_HTTP, host);
  TEST_ASSERT_EQUAL_INT(atoi(PORT), port);
}

void test_wrong_uri(void) {
  char *ipv6_wrong_case1 = PROTOCOL_TCP "://" HOST_IPv6 ":" PORT;
  char *ipv6_wrong_case2 = PROTOCOL_TCP "://[" HOST_IPv6 "]" PORT;
  char *ipv6_wrong_case3 = "ldm://[" HOST_IPv6 "]" PORT;
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
