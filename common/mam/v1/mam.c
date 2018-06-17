#include <stdio.h>
#include <string.h>

#include "common/curl-p/hamming.h"
#include "common/sign/v2/iss_curl.h"
#include "common/trinary/trit_long.h"
#include "mam/mask.h"
#include "mam/merkle.h"

int mam_init_encryption(trit_t *side_key, size_t side_key_length,
                        trit_t *merkle_root, Curl *enc_curl) {
  curl_absorb(enc_curl, side_key, side_key_length);
  curl_absorb(enc_curl, merkle_root, HASH_LENGTH);
  return 0;
}

int payload_min_length(size_t message_length, size_t merkle_tree_length,
                       size_t index, size_t security) {
  size_t sibling_number = merkle_depth(merkle_tree_length / HASH_LENGTH) - 1;
  return encoded_length(index) + encoded_length(message_length) + HASH_LENGTH +
         message_length + (HASH_LENGTH / 3) + security * ISS_KEY_LENGTH +
         encoded_length(sibling_number) + (sibling_number * HASH_LENGTH);
}

int mam_create(trit_t *payload, size_t payload_length, trit_t *message,
               size_t message_length, trit_t *side_key, size_t side_key_length,
               trit_t *merkle_tree, size_t merkle_tree_length,
               size_t leaf_count, size_t index, size_t start, trit_t *next_root,
               trit_t *seed, size_t security, Curl *enc_curl) {
  if (security > 3) {
    fprintf(stderr, "invalid security %zd\n", security);
    return -1;
  }

  size_t sibling_number = merkle_depth(merkle_tree_length / HASH_LENGTH) - 1;
  size_t enc_index_length = encoded_length(index);
  size_t enc_message_length_length = encoded_length(message_length);
  size_t signature_length = security * ISS_KEY_LENGTH;
  size_t enc_siblings_number_length = encoded_length(sibling_number);
  size_t payload_min_length = enc_index_length + enc_message_length_length +
                              HASH_LENGTH + message_length + (HASH_LENGTH / 3) +
                              signature_length + enc_siblings_number_length +
                              (sibling_number * HASH_LENGTH);

  if (payload_length < payload_min_length) {
    fprintf(stderr, "payload too short: needed %zd, given %zd\n",
            payload_min_length, payload_length);
    return -1;
  }

  size_t offset = 0;

  mam_init_encryption(side_key, side_key_length, merkle_tree, enc_curl);

  // encode index to payload
  encode_long(index, payload + offset, enc_index_length);
  offset += enc_index_length;

  // encode message length to payload
  encode_long(message_length, payload + offset, enc_message_length_length);
  offset += enc_message_length_length;

  curl_absorb(enc_curl, payload, offset);

  // encrypt next root to payload
  mask(payload + offset, next_root, HASH_LENGTH, enc_curl);
  offset += HASH_LENGTH;

  // encrypt message to payload
  mask(payload + offset, message, message_length, enc_curl);
  offset += message_length;

  // encrypt nonce to payload
  Curl curl;
  curl.type = CURL_P_27;
  memcpy(curl.state, enc_curl->state, sizeof(enc_curl->state));
  hamming(&curl, 0, HASH_LENGTH / 3, security);
  mask(payload + offset, curl.state, HASH_LENGTH / 3, enc_curl);
  offset += HASH_LENGTH / 3;

  // encrypt signature to payload
  curl_reset(&curl);
  iss_curl_subseed(seed, payload + offset, start + index, &curl);
  iss_curl_key(payload + offset, payload + offset, signature_length, &curl);
  iss_curl_signature(payload + offset, enc_curl->state, 0, payload + offset,
                     signature_length, &curl);
  offset += signature_length;

  // encrypt siblings number to payload
  encode_long(sibling_number, payload + offset, enc_siblings_number_length);
  offset += enc_siblings_number_length;

  // encrypt siblings to payload
  merkle_branch(payload + offset, merkle_tree, merkle_tree_length,
                sibling_number + 1, index, leaf_count);
  offset += sibling_number * HASH_LENGTH;

  size_t to_mask = signature_length + enc_siblings_number_length +
                   sibling_number * HASH_LENGTH;
  mask(payload + offset - to_mask, payload + offset - to_mask, to_mask,
       enc_curl);

  curl_reset(enc_curl);

  return payload_min_length;
}

int mam_parse(trit_t *payload, size_t payload_length, trit_t *message,
              size_t *message_length, trit_t *side_key, size_t side_key_length,
              trit_t *root, size_t *index, trit_t *next_root, size_t *security,
              Curl *enc_curl) {
  size_t offset = 0;

  mam_init_encryption(side_key, side_key_length, root, enc_curl);

  // decode index from payload
  if (offset >= payload_length) return -1;
  size_t enc_index_length;
  *index =
      decode_long(payload + offset, payload_length - offset, &enc_index_length);
  offset += enc_index_length;

  // decode message length from payload
  if (offset >= payload_length) return -1;
  size_t enc_message_length_length;
  *message_length = decode_long(payload + offset, payload_length - offset,
                                &enc_message_length_length);
  offset += enc_message_length_length;

  if (offset >= payload_length) return -1;
  curl_absorb(enc_curl, payload, offset);

  // decrypt next root from payload
  unmask(next_root, payload + offset, HASH_LENGTH, enc_curl);
  offset += HASH_LENGTH;

  // decrypt message from payload
  if (offset >= payload_length) return -1;
  unmask(message, payload + offset, *message_length, enc_curl);
  offset += *message_length;

  // decrypt nonce from payload
  if (offset >= payload_length) return -1;
  unmask(payload + offset, payload + offset, HASH_LENGTH / 3, enc_curl);
  offset += HASH_LENGTH / 3;

  // get security back from state
  if (offset >= payload_length) return -1;
  trit_t hash[HASH_LENGTH];
  memcpy(hash, enc_curl->state, HASH_LENGTH * sizeof(trit_t));
  *security = signed_window(hash);
  unmask(payload + offset, payload + offset, payload_length - offset, enc_curl);

  if (*security == 0)
    return -1;
  else {
    // decrypt signature from payload
    curl_reset(enc_curl);
    iss_curl_sig_digest(hash, hash, 0, payload + offset,
                        *security * ISS_KEY_LENGTH, enc_curl);

    // complete the address
    curl_reset(enc_curl);
    iss_curl_address(hash, hash, HASH_LENGTH, enc_curl);
    offset += *security * ISS_KEY_LENGTH;

    // decrypt siblings number from payload
    if (offset >= payload_length) return -1;
    size_t enc_sibling_number_length;
    size_t sibling_number = decode_long(
        payload + offset, payload_length - offset, &enc_sibling_number_length);
    offset += enc_sibling_number_length;

    // get merkle root from siblings from payload
    if (sibling_number != 0) {
      if (offset >= payload_length) return -1;
      curl_reset(enc_curl);
      merkle_root(hash, payload + offset, sibling_number, *index, enc_curl);
    }

    // check merkle root with the given root
    if (memcmp(hash, root, HASH_LENGTH * sizeof(trit_t)) == 0)
      return 0;
    else
      return -1;
  }
  return 0;
}
