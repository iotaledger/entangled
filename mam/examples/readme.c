#include <stdlib.h>

#include "mam/api/api.h"

int main() {
  /**
   * Initialize the API
   */

  mam_api_t api;

  mam_api_init(&api, (tryte_t *)"MAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLEMAMEXAMPLE9");

  /**
   * Create a channel
   */

  tryte_t channel_id[MAM_CHANNEL_ID_SIZE];

  mam_api_create_channel(&api, 5, channel_id);

  fprintf(stderr, "Channel ID: ");
  for (size_t i = 0; i < MAM_CHANNEL_ID_SIZE / 3; i++) {
    fprintf(stderr, "%c", channel_id[i]);
  }
  fprintf(stderr, "\n");

  /**
   * Create an endpoint
   */

  tryte_t endpoint_id[MAM_ENDPOINT_ID_SIZE];

  mam_api_create_endpoint(&api, 5, channel_id, endpoint_id);

  fprintf(stderr, "Endpoint ID: ");
  for (size_t i = 0; i < MAM_ENDPOINT_ID_SIZE / 3; i++) {
    fprintf(stderr, "%c", endpoint_id[i]);
  }
  fprintf(stderr, "\n");

  /**
   * Write a header to a bundle
   */

  bundle_transactions_t *bundle = NULL;
  trit_t message_id[MAM_MSG_ID_SIZE];

  bundle_transactions_new(&bundle);
  mam_api_bundle_write_header_on_endpoint(&api, channel_id, endpoint_id, NULL, NULL, bundle, message_id);

  /**
   * Write a packet to a bundle
   */

  mam_api_bundle_write_packet(&api, message_id, (tryte_t *)"PAYLOAD", 7, MAM_MSG_CHECKSUM_SIG, true, bundle);

  /**
   * Read a packet from a bundle
   */

  tryte_t *payload = NULL;
  size_t payload_size = 0;
  bool is_last_packet = false;

  mam_api_bundle_read(&api, bundle, &payload, &payload_size, &is_last_packet);
  fprintf(stderr, "Payload: ");
  for (size_t i = 0; i < payload_size; i++) {
    fprintf(stderr, "%c", payload[i]);
  }
  fprintf(stderr, "\n");
  free(payload);

  /**
   * Destroy the API
   */

  mam_api_destroy(&api);

  return EXIT_SUCCESS;
}
