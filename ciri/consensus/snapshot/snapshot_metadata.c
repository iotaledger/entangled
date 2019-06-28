/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/snapshot/snapshot_metadata.h"
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

void iota_snapshot_metadata_reset(snapshot_metadata_t *const snapshot_metadata) {
  snapshot_metadata->index = 0;
  snapshot_metadata->timestamp = 0;
  snapshot_metadata->solid_entry_points = NULL;
}

retcode_t iota_snapshot_metadata_destroy(snapshot_metadata_t *const snapshot_metadata) {
  hash_to_uint64_t_map_free(&snapshot_metadata->solid_entry_points);
  return RC_OK;
}

size_t iota_snapshot_metadata_serialized_str_size(snapshot_metadata_t const *const snapshot_metadata) {
  // hash, index, timestamps, each are followed by new line character
  // each solid entry point map entry is two fields delimited by ';' and ended with a new line character
  return NUM_TRYTES_HASH + 1 + MAX_CHARS_UINT64 + 1 + MAX_CHARS_UINT64 + 1 +
         hash_to_uint64_t_map_size(snapshot_metadata->solid_entry_points) *
             (MAX_CHARS_UINT64 + 1 + NUM_TRYTES_HASH + 1);
}

retcode_t iota_snapshot_metadata_serialize_str(snapshot_metadata_t const *const snapshot_metadata, char *const str) {
  hash_to_uint64_t_map_entry_t *iter = NULL, *tmp = NULL;
  size_t value_len;
  size_t offset = 0;

  if (flex_trits_to_trytes((tryte_t *)(str + offset), NUM_TRYTES_HASH, snapshot_metadata->hash, NUM_TRITS_HASH,
                           NUM_TRITS_HASH) != NUM_TRITS_HASH) {
    return RC_SNAPSHOT_METADATA_FAILED_SERIALIZING;
  }

  offset += NUM_TRYTES_HASH;
  str[offset] = '\n';
  offset += 1;
  value_len = sprintf(str + offset, "%" PRIu64 "\n", snapshot_metadata->index);
  offset += value_len;
  value_len = sprintf(str + offset, "%" PRIu64 "\n", snapshot_metadata->timestamp);
  offset += value_len;

  HASH_ITER(hh, snapshot_metadata->solid_entry_points, iter, tmp) {
    if (flex_trits_to_trytes((tryte_t *)(str + offset), NUM_TRYTES_ADDRESS, iter->hash, NUM_TRITS_HASH,
                             NUM_TRITS_HASH) != NUM_TRITS_HASH) {
      return RC_SNAPSHOT_METADATA_FAILED_SERIALIZING;
    }
    offset += NUM_TRYTES_ADDRESS;
    str[offset] = ';';
    offset += 1;
    value_len = sprintf(str + offset, "%" PRIu64 "\n", iter->value);
    offset += value_len;
  }

  str[offset] = '\0';

  return RC_OK;
}
retcode_t iota_snapshot_metadata_deserialize_str(char const *const str, snapshot_metadata_t *const snapshot_metadata) {
  retcode_t ret;
  char c;
  flex_trit_t curr_hash[FLEX_TRIT_SIZE_243];
  uint64_t snapshot_index;
  int offset;
  char const *ptr = str;
  char *token;

  if (flex_trits_from_trytes(snapshot_metadata->hash, NUM_TRITS_HASH, (tryte_t *)ptr, NUM_TRYTES_HASH,
                             NUM_TRYTES_HASH) != NUM_TRYTES_HASH) {
    return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
  }
  ptr += NUM_TRYTES_HASH;

  c = ptr[0];
  ptr += 1;
  if (sscanf(ptr, "%" PRIu64 "%n", &snapshot_metadata->index, &offset) != 1) {
    return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
  }
  ptr += offset;
  c = ptr[0];
  ptr += 1;
  if (sscanf(ptr, "%" PRIu64 "%n", &snapshot_metadata->timestamp, &offset) != 1) {
    return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
  }
  ptr += offset;
  c = ptr[0];
  ptr += 1;

  token = strtok((char *)ptr, ";");
  while (token != NULL) {
    if (flex_trits_from_trytes(curr_hash, NUM_TRITS_ADDRESS, (tryte_t *)token, NUM_TRYTES_ADDRESS,
                               NUM_TRYTES_ADDRESS) != NUM_TRYTES_ADDRESS) {
      return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
    }

    if ((token = strtok(NULL, "\n")) == NULL) {
      break;
    }
    if (sscanf(token, "%" PRIu64 "", &snapshot_index) != 1) {
      return RC_SNAPSHOT_METADATA_FAILED_DESERIALIZING;
    }

    ERR_BIND_RETURN(hash_to_uint64_t_map_add(&snapshot_metadata->solid_entry_points, curr_hash, snapshot_index), ret);
    token = strtok(NULL, ";");
  }

  return RC_OK;
}

retcode_t iota_snapshot_metadata_read_from_file(snapshot_metadata_t *const snapshot_metadata,
                                                char const *const metadata_file) {
  retcode_t ret = RC_OK;
  char *buffer = NULL;

  ERR_BIND_GOTO(iota_utils_read_file_into_buffer(metadata_file, &buffer), ret, cleanup);
  if (buffer) {
    ERR_BIND_GOTO(iota_snapshot_metadata_deserialize_str(buffer, snapshot_metadata), ret, cleanup);
  }

cleanup:
  if (buffer) {
    free(buffer);
  }

  return ret;
}
