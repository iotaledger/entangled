/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/snapshot_metadata.h"
#include <inttypes.h>
#include <stdlib.h>
#include "common/model/transaction.h"
#include "utils/files.h"
#include "utils/macros.h"

retcode_t iota_snapshot_metadata_init(snapshot_metadata_t *const snapshot_metadata, flex_trit_t const *const hash,
                                      uint64_t index, uint64_t timestamp, hash_to_uint64_t_map_t solid_entry_points) {
  memcpy(snapshot_metadata->hash, hash, FLEX_TRIT_SIZE_243);
  snapshot_metadata->index = index;
  snapshot_metadata->timestamp = timestamp;
  snapshot_metadata->solid_entry_points = NULL;
  hash_to_uint64_t_map_copy(&solid_entry_points, &snapshot_metadata->solid_entry_points);
  return RC_OK;
}

retcode_t iota_snapshot_metadata_destroy(snapshot_metadata_t *const snapshot_metadata) {
  hash_to_uint64_t_map_free(&snapshot_metadata->solid_entry_points);
  return RC_OK;
}

size_t iota_snapshot_metadata_serialized_str_size(snapshot_metadata_t const *const snapshot_metadata) {
  // hash, index, timestamps, solid entries map size, each are followed by new line character
  // each solid entry point map entry is two fields delimited by ';' and ended with a new line character
  return FLEX_TRIT_SIZE_243 + 1 + MAX_CHARS_UNIT64 + 1 + MAX_CHARS_UNIT64 + 1 + MAX_CHARS_UNIT64 + 1 +
         hash_to_uint64_t_map_size(snapshot_metadata->solid_entry_points) *
             (MAX_CHARS_UNIT64 + 1 + FLEX_TRIT_SIZE_243 + 1);
}

retcode_t iota_snapshot_metadata_serialize_str(snapshot_metadata_t const *const snapshot_metadata, char *const str) {
  hash_to_uint64_t_map_entry_t *iter = NULL, *tmp = NULL;
  tryte_t hash_trytes[NUM_TRYTES_HASH];
  char svalue[MAX_CHARS_UNIT64];

  flex_trits_to_trytes(&hash_trytes, NUM_TRYTES_ADDRESS, snapshot_metadata->hash, NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);
  snprintf(str, NUM_TRYTES_ADDRESS + 1, "%s", hash_trytes);
  strcat(str, "\n");
  sprintf(svalue, "%" PRIu64 "\n", snapshot_metadata->index);
  strcat(str, svalue);
  sprintf(svalue, "%" PRIu64 "\n", snapshot_metadata->timestamp);
  strcat(str, svalue);

  HASH_ITER(hh, snapshot_metadata->solid_entry_points, iter, tmp) {
    if (flex_trits_to_trytes(&hash_trytes, NUM_TRYTES_ADDRESS, iter->hash, NUM_TRITS_HASH, NUM_TRITS_HASH) !=
        NUM_TRITS_HASH) {
      return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
    }
    strncat(str, hash_trytes, NUM_TRYTES_ADDRESS);
    strcat(str, ";");
    sprintf(svalue, "%" PRIu64 "\n", iter->value);
    strcat(str, svalue);
  }

  return RC_OK;
}
retcode_t iota_snapshot_metadata_deserialize_str(char const *const str, snapshot_metadata_t *const snapshot_metadata) {
  retcode_t ret;
  char c;
  tryte_t curr_hash_trytes[81];
  flex_trit_t curr_hash[FLEX_TRIT_SIZE_243];
  uint64_t snapshot_index;
  int offset;
  char const *ptr = str;
  char *token;

  if (sscanf(ptr, "%s%n", snapshot_metadata->hash, &offset) != 1) {
    return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
  }
  ptr += offset;
  if (sscanf(ptr, "%c%n", &c, &offset) != 1) {
    return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
  }
  ptr += offset;
  if (sscanf(ptr, "%" PRIu64 "%n", &snapshot_metadata->index, &offset) != 1) {
    return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
  }
  ptr += offset;
  if (sscanf(ptr, "%c%n", &c, &offset) != 1) {
    return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
  }
  ptr += offset;
  if (sscanf(ptr, "%" PRIu64 "%n", &snapshot_metadata->timestamp, &offset) != 1) {
    return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
  }
  ptr += offset;
  if (sscanf(ptr, "%c%n", &c, &offset) != 1) {
    return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
  }
  ptr += offset;

  token = strtok(ptr, ";");
  while (token != NULL) {
    strcpy(curr_hash_trytes, token);

    if ((token = strtok(NULL, "\n")) == NULL) {
      return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
    }
    if (sscanf(token, "%" PRIu64 "", &snapshot_index) != 1) {
      return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
    }

    if (flex_trits_from_trytes(curr_hash, NUM_TRITS_HASH, curr_hash_trytes, NUM_TRYTES_HASH, NUM_TRYTES_HASH) !=
        NUM_TRYTES_HASH) {
      return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
    }
    ERR_BIND_RETURN(hash_to_uint64_t_map_add(&snapshot_metadata->solid_entry_points, curr_hash, snapshot_index), ret);
    token = strtok(NULL, ";");
  }

  return RC_OK;
}

retcode_t iota_snapshot_metadata_read_from_file(snapshot_metadata_t *const metadata, char const *const metadata_file) {
  retcode_t ret;
  char *buffer = NULL;

  ERR_BIND_GOTO(iota_utils_read_file_into_buffer(metadata_file, &buffer), ret, cleanup);
  if (buffer) {
    ERR_BIND_GOTO(iota_snapshot_metadata_deserialize_str(buffer, metadata), ret, cleanup);
  }

cleanup:
  if (buffer) {
    free(buffer);
  }

  return ret;
}