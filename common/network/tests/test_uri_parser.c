#include <stdio.h>
#include <unity/unity.h>
#include "common/network/uri_parser.h"

void test_uri_parser_ipv4(void) {
  char *uri_ipv4 = "tcp://8.8.8.8:4242";
  char *scheme;
  size_t scheme_len = 5;
  char *host;
  size_t host_len = 17;
  uint16_t *port;

  scheme = (char *)malloc(sizeof(char) * scheme_len);
  host = (char *)malloc(sizeof(char) * host_len);
  port = (uint16_t *)malloc(sizeof(uint16_t));

  TEST_ASSERT_TRUE(
      uri_parse(uri_ipv4, scheme, scheme_len, host, host_len, port));
  TEST_ASSERT_EQUAL_STRING("tcp", scheme);
  TEST_ASSERT_EQUAL_STRING("8.8.8.8", host);
  TEST_ASSERT_EQUAL_INT(4242, *port);
}

void test_uri_parser_ipv6(void) {
  char *uri_ipv6 = "tcp://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:4242";
  char *scheme;
  size_t scheme_len = 5;
  char *host;
  size_t host_len = 40;
  uint16_t *port;

  scheme = (char *)malloc(sizeof(char) * scheme_len);
  host = (char *)malloc(sizeof(char) * host_len);
  port = (uint16_t *)malloc(sizeof(uint16_t));

  TEST_ASSERT_TRUE(
      uri_parse(uri_ipv6, scheme, scheme_len, host, host_len, port));
  TEST_ASSERT_EQUAL_STRING("tcp", scheme);
  TEST_ASSERT_EQUAL_STRING("FEDC:BA98:7654:3210:FEDC:BA98:7654:3210", host);
  TEST_ASSERT_EQUAL_INT(4242, *port);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_uri_parser_ipv4);
  RUN_TEST(test_uri_parser_ipv6);

  return UNITY_END();
}