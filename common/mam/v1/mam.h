#include <mam/mask.h>
#include "common/curl-p/hamming.h"
#include "common/trinary/trit_long.h"

int mam_init_encryption(trit_t *encryption_key, size_t key_length, trit_t *r,
                        Curl *c);

int payload_min_length(size_t message_length, size_t merkle_tree_length,
                       size_t index, size_t security);

int mam_create(trit_t *payload, size_t payload_length, trit_t *message,
               size_t message_length, trit_t *side_key, size_t side_key_length,
               trit_t *merkle_tree, size_t merkle_tree_length, size_t index,
               size_t start, trit_t *next_root, trit_t *seed, size_t security,
               Curl *enc_curl);

int mam_parse(trit_t *payload, size_t payload_length, trit_t *message,
              size_t *message_length, trit_t *side_key, size_t side_key_length,
              trit_t *root, size_t *index, trit_t *next_root, size_t *security,
              Curl *enc_curl);
