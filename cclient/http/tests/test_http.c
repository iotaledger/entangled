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

const char* data =
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

void test_http(void) {
  iota_client_service_t service = {{0}};
  service.http.host = "httpbin.org";
  service.http.port = 80;
  service.http.path = "/post";
  char_buffer_t* req = char_buffer_new();
  char_buffer_t* res = char_buffer_new();
  char_buffer_allocate(req, strlen(data));
  memcpy(req->data, data, req->length);
  iota_service_query(&service, req, res);
  cJSON* json_obj = cJSON_Parse(res->data);
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "form");
  // For some reason the server echoes the data into the key of the json object
  char* string = json_item->child->string;
  TEST_ASSERT_EQUAL_STRING(data, string);
  cJSON_Delete(json_obj);
  char_buffer_free(req);
  char_buffer_free(res);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_http);

  return UNITY_END();
}
