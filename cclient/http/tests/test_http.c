/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "cJSON.h"
#include "cclient/http/http.h"

static char const* amazon_ca1_pem =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\r\n"
    "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\r\n"
    "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\r\n"
    "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\r\n"
    "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\r\n"
    "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\r\n"
    "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\r\n"
    "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\r\n"
    "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\r\n"
    "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\r\n"
    "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\r\n"
    "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\r\n"
    "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\r\n"
    "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\r\n"
    "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\r\n"
    "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\r\n"
    "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\r\n"
    "rqXRfboQnoZsG4q5WTP468SQvvG5\r\n"
    "-----END CERTIFICATE-----\r\n";

static char const* data =
    "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Phasellus "
    "hendrerit. Pellentesque aliquet nibh nec urna. In nisi neque, aliquet "
    "vel, dapibus id, mattis vel, nisi. Sed pretium, ligula sollicitudin "
    "laoreet viverra, tortor libero sodales leo, eget blandit nunc tortor eu "
    "nibh. Nullam mollis. Ut justo. Suspendisse potenti. Sed egestas, ante et "
    "vulputate volutpat, eros pede semper est, vitae luctus metus libero eu "
    "augue. Morbi purus libero, faucibus adipiscing, commodo quis, gravida id, "
    "est. Sed lectus. Praesent elementum hendrerit tortor. Sed semper lorem at "
    "felis. Vestibulum volutpat, lacus a ultrices sagittis, mi neque euismod "
    "dui, eu pulvinar nunc sapien ornare nisl. Phasellus pede arcu, dapibus "
    "eu, fermentum et, dapibus sed, urna. Morbi interdum mollis sapien. Sed ac "
    "risus. Phasellus lacinia, magna a ullamcorper laoreet, lectus arcu "
    "pulvinar risus, vitae facilisis libero dolor a purus. Sed vel lacus. "
    "Mauris nibh felis, adipiscing varius, adipiscing in, lacinia vel, tellus. "
    "Suspendisse ac urna. Etiam pellentesque mauris ut lectus. Nunc tellus "
    "ante, mattis eget, gravida vitae, ultricies ac, leo. Integer leo pede, "
    "ornare a, lacinia eu, vulputate vel, nisl. Suspendisse mauris. Fusce "
    "accumsan mollis eros. Pellentesque a diam sit amet mi ullamcorper "
    "vehicula. Integer adipiscing risus a sem. Nullam quis massa sit amet nibh "
    "viverra malesuada. Nunc sem lacus, accumsan quis, faucibus non, congue "
    "vel, arcu. Ut scelerisque hendrerit tellus. Integer sagittis. Vivamus a "
    "mauris eget arcu gravida tristique. Nunc iaculis mi in ante. Vivamus "
    "imperdiet nibh feugiat est. Ut convallis, sem sit amet interdum "
    "consectetuer, odio augue aliquam leo, nec dapibus tortor nibh sed augue. "
    "Integer eu magna sit amet metus fermentum posuere. Morbi sit amet nulla "
    "sed dolor elementum imperdiet. Quisque fermentum. Cum sociis natoque "
    "penatibus et magnis xdis parturient montes, nascetur ridiculus mus. "
    "Pellentesque adipiscing eros ut libero. Ut condimentum mi vel tellus. "
    "Suspendisse laoreet. Fusce ut est sed dolor gravida convallis. Morbi "
    "vitae ante. Vivamus ultrices luctus nunc. Suspendisse et dolor. Etiam "
    "dignissim. Proin malesuada adipiscing lacus. Donec metus. Lorem ipsum "
    "dolor sit amet, consectetuer adipiscing elit. Phasellus hendrerit. "
    "Pellentesque aliquet nibh nec urna. In nisi neque, aliquet vel, dapibus "
    "id, mattis vel, nisi. Sed pretium, ligula sollicitudin laoreet viverra, "
    "tortor libero sodales leo, eget blandit nunc tortor eu nibh. Nullam "
    "mollis. Ut justo. Suspendisse potenti. Sed egestas, ante et vulputate "
    "volutpat, eros pede semper est, vitae luctus metus libero eu augue. Morbi "
    "purus libero, faucibus adipiscing, commodo quis, gravida id, est. Sed "
    "lectus. Praesent elementum hendrerit tortor. Sed semper lorem at felis. "
    "Vestibulum volutpat, lacus a ultrices sagittis, mi neque euismod dui, eu "
    "pulvinar nunc sapien ornare nisl. Phasellus pede arcu, dapibus eu, "
    "fermentum et, dapibus sed, urna. Morbi interdum mollis sapien. Sed ac "
    "risus. Phasellus lacinia, magna a ullamcorper laoreet, lectus arcu "
    "pulvinar risus, vitae facilisis libero dolor a purus. Sed vel lacus. "
    "Mauris nibh felis, adipiscing varius, adipiscing in, lacinia vel, tellus. "
    "Suspendisse ac urna. Etiam pellentesque mauris ut lectus. Nunc tellus "
    "ante, mattis eget, gravida vitae, ultricies ac, leo. Integer leo pede, "
    "ornare a, lacinia eu, vulputate vel, nisl. Suspendisse mauris. Fusce "
    "accumsan mollis eros. Pellentesque a diam sit amet mi ullamcorper "
    "vehicula. Integer adipiscing risus a sem. Nullam quis massa sit amet nibh "
    "viverra malesuada. Nunc sem lacus, accumsan quis, faucibus non, congue "
    "vel, arcu. Ut scelerisque hendrerit tellus. Integer sagittis. Vivamus a "
    "mauris eget arcu gravida tristique. Nunc iaculis mi in ante. Vivamus "
    "imperdiet nibh feugiat est. Ut convallis, sem sit amet interdum "
    "consectetuer, odio augue aliquam leo, nec dapibus tortor nibh sed augue. "
    "Integer eu magna sit amet metus fermentum posuere. Morbi sit amet nulla "
    "sed dolor elementum imperdiet. Quisque fermentum. Cum sociis natoque "
    "penatibus et magnis xdis parturient montes, nascetur ridiculus mus. "
    "Pellentesque adipiscing eros ut libero. Ut condimentum mi vel tellus. "
    "Suspendisse laoreet. Fusce ut est sed dolor gravida convallis. Morbi "
    "vitae ante. Vivamus ultrices luctus nunc. Suspendisse et dolor. Etiam "
    "dignissim. Proin malesuada adipiscing lacus. Donec metus. Lorem ipsum "
    "dolor sit amet, consectetuer adipiscing elit. Phasellus hendrerit. "
    "Pellentesque aliquet nibh nec urna. In nisi neque, aliquet vel, dapibus "
    "id, mattis vel, nisi. Sed pretium, ligula sollicitudin laoreet viverra, "
    "tortor libero sodales leo, eget blandit nunc tortor eu nibh. Nullam "
    "mollis. Ut justo. Suspendisse potenti. Sed egestas, ante et vulputate "
    "volutpat, eros pede semper est, vitae luctus metus libero eu augue. Morbi "
    "purus libero, faucibus adipiscing, commodo quis, gravida id, est. Sed "
    "lectus. Praesent elementum hendrerit tortor. Sed semper lorem at felis. "
    "Vestibulum volutpat, lacus a ultrices sagittis, mi neque euismod dui, eu "
    "pulvinar nunc sapien ornare nisl. Phasellus pede arcu, dapibus eu, "
    "fermentum et, dapibus sed, urna. Morbi interdum mollis sapien. Sed ac "
    "risus. Phasellus lacinia, magna a ullamcorper laoreet, lectus arcu "
    "pulvinar risus, vitae facilisis libero dolor a purus. Sed vel lacus. "
    "Mauris nibh felis, adipiscing varius, adipiscing in, lacinia vel, tellus. "
    "Suspendisse ac urna. Etiam pellentesque mauris ut lectus. Nunc tellus "
    "ante, mattis eget, gravida vitae, ultricies ac, leo. Integer leo pede, "
    "ornare a, lacinia eu, vulputate vel, nisl. Suspendisse mauris. Fusce "
    "accumsan mollis eros. Pellentesque a diam sit amet mi ullamcorper "
    "vehicula. Integer adipiscing risus a sem. Nullam quis massa sit amet nibh "
    "viverra malesuada. Nunc sem lacus, accumsan quis, faucibus non, congue "
    "vel, arcu. Ut scelerisque hendrerit tellus. Integer sagittis. Vivamus a "
    "mauris eget arcu gravida tristique. Nunc iaculis mi in ante. Vivamus "
    "imperdiet nibh feugiat est. Ut convallis, sem sit amet interdum "
    "consectetuer, odio augue aliquam leo, nec dapibus tortor nibh sed augue. "
    "Integer eu magna sit amet metus fermentum posuere. Morbi sit amet nulla "
    "sed dolor elementum imperdiet. Quisque fermentum. Cum sociis natoque "
    "penatibus et magnis xdis parturient montes, nascetur ridiculus mus. "
    "Pellentesque adipiscing eros ut libero. Ut condimentum mi vel tellus. "
    "Suspendisse laoreet. Fusce ut est sed dolor gravida convallis. Morbi "
    "vitae ante. Vivamus ultrices luctus nunc. Suspendisse et dolor. Etiam "
    "dignissim. Proin malesuada adipiscing lacus.";

void test_http_request(void) {
  iota_client_service_t service = {};
  service.http.host = "httpbin.org";
  service.http.content_type = khttp_ApplicationFormUrlencoded;
  service.http.accept = khttp_ApplicationJson;
  service.http.port = 80;
  service.http.path = "/post";
  service.http.ca_pem = NULL;
  char_buffer_t* req = char_buffer_new();
  char_buffer_t* res = char_buffer_new();
  TEST_ASSERT_NOT_NULL(req);
  TEST_ASSERT_NOT_NULL(res);
  char_buffer_allocate(req, strlen(data));
  memcpy(req->data, data, req->length);
  TEST_ASSERT(iota_service_query(&service, req, res) == RC_OK);
  cJSON* json_obj = cJSON_Parse(res->data);
  TEST_ASSERT_NOT_NULL(json_obj);
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "form");
  TEST_ASSERT_NOT_NULL(json_item);
  // For some reason the server echoes the data into the key of the json object
  char* string = json_item->child->string;
  TEST_ASSERT_NOT_NULL(string);
  TEST_ASSERT_EQUAL_STRING(data, string);

  cJSON_Delete(json_obj);
  char_buffer_free(req);
  char_buffer_free(res);
}

void test_https_request(void) {
  iota_client_service_t service = {};
  service.http.host = "postman-echo.com";
  service.http.content_type = khttp_ApplicationFormUrlencoded;
  service.http.accept = khttp_ApplicationJson;
  service.http.port = 443;
  service.http.path = "/post";
  service.http.ca_pem = amazon_ca1_pem;
  char_buffer_t* req = char_buffer_new();
  char_buffer_t* res = char_buffer_new();
  TEST_ASSERT_NOT_NULL(req);
  TEST_ASSERT_NOT_NULL(res);
  char_buffer_allocate(req, strlen(data));
  memcpy(req->data, data, req->length);
  TEST_ASSERT(iota_service_query(&service, req, res) == RC_OK);
  cJSON* json_obj = cJSON_Parse(res->data);
  TEST_ASSERT_NOT_NULL(json_obj);
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "form");
  TEST_ASSERT_NOT_NULL(json_item);
  // For some reason the server echoes the data into the key of the json object
  char* string = json_item->child->string;
  TEST_ASSERT_NOT_NULL(string);
  TEST_ASSERT_EQUAL_STRING(data, string);

  cJSON_Delete(json_obj);
  char_buffer_free(req);
  char_buffer_free(res);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_http_request);
  RUN_TEST(test_https_request);

  return UNITY_END();
}
