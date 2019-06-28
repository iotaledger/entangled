/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/snapshot/state_delta.h"
#include <inttypes.h>
#include <stdlib.h>
#include "ciri/consensus/conf.h"
#include "common/model/transaction.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"

int64_t state_delta_sum(state_delta_t const *const state) {
  int64_t sum = 0;
  state_delta_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *state, iter, tmp) { sum += iter->value; }

  return sum;
}

retcode_t state_delta_add_or_sum(state_delta_t *const state, flex_trit_t const *const hash, int64_t const value) {
  retcode_t ret = RC_OK;
  state_delta_entry_t *entry = NULL;

  if (value == 0) {
    return RC_OK;
  }

  state_delta_find(*state, hash, entry);
  if (entry) {
    entry->value += value;
    if (entry->value == 0) {
      state_delta_remove(state, hash);
    }
  } else {
    if ((ret = state_delta_add(state, hash, value)) != RC_OK) {
      return ret;
    }
  }

  return RC_OK;
}

retcode_t state_delta_sum_if_present(state_delta_t *const state, flex_trit_t const *const hash, int64_t const value) {
  state_delta_entry_t *entry = NULL;

  if (value == 0) {
    return RC_OK;
  }

  state_delta_find(*state, hash, entry);
  if (entry) {
    entry->value += value;
  }

  return RC_OK;
}

retcode_t state_delta_add_or_replace(state_delta_t *const state, flex_trit_t const *const hash, int64_t const value) {
  retcode_t ret = RC_OK;
  state_delta_entry_t *entry = NULL;

  state_delta_find(*state, hash, entry);
  if (entry) {
    if (value == 0) {
      state_delta_remove(state, hash);
    } else {
      entry->value = value;
    }
  } else {
    if ((ret = state_delta_add(state, hash, value)) != RC_OK) {
      return ret;
    }
  }

  return RC_OK;
}

retcode_t state_delta_create_patch(state_delta_t const *const state, state_delta_t const *const delta,
                                   state_delta_t *const patch) {
  retcode_t ret = RC_OK;
  state_delta_entry_t *iter = NULL, *entry = NULL, *tmp = NULL;

  HASH_ITER(hh, *delta, iter, tmp) {
    state_delta_find(*state, iter->hash, entry);
    if ((ret = state_delta_add(patch, iter->hash, (entry ? entry->value : 0) + iter->value)) != RC_OK) {
      return ret;
    }
  }

  return ret;
}

retcode_t state_delta_apply_patch(state_delta_t *const state, state_delta_t const *const patch) {
  retcode_t ret = RC_OK;
  state_delta_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *patch, iter, tmp) {
    if (iter->value != 0) {
      if ((ret = state_delta_add_or_sum(state, iter->hash, iter->value)) != RC_OK) {
        return ret;
      }
    }
  }

  return ret;
}

retcode_t state_delta_apply_patch_if_present(state_delta_t *const state, state_delta_t const *const patch) {
  retcode_t ret = RC_OK;
  state_delta_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *patch, iter, tmp) {
    if ((ret = state_delta_sum_if_present(state, iter->hash, iter->value)) != RC_OK) {
      return ret;
    }
  }

  return ret;
}

retcode_t state_delta_merge_patch(state_delta_t *const state, state_delta_t const *const patch) {
  retcode_t ret = RC_OK;
  state_delta_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *patch, iter, tmp) {
    if ((ret = state_delta_add_or_replace(state, iter->hash, iter->value)) != RC_OK) {
      return ret;
    }
  }

  return ret;
}

bool state_delta_is_consistent(state_delta_t *const delta) {
  state_delta_entry_t *entry, *tmp;

  HASH_ITER(hh, *delta, entry, tmp) {
    if (entry->value <= 0) {
      if (entry->value < 0) {
        return false;
      }
      HASH_DEL(*delta, entry);
      free(entry);
    }
  }

  return true;
}

size_t state_delta_serialized_size(state_delta_t const *const delta) {
  if (delta == NULL) {
    return 0;
  }

  return HASH_COUNT(*delta) * (FLEX_TRIT_SIZE_243 + sizeof(int64_t));
}

retcode_t state_delta_serialize(state_delta_t const *const delta, byte_t *const bytes) {
  uint64_t offset = 0;
  state_delta_entry_t *iter = NULL, *tmp = NULL;

  HASH_ITER(hh, *delta, iter, tmp) {
    memcpy(bytes + offset, iter->hash, FLEX_TRIT_SIZE_243);
    offset += FLEX_TRIT_SIZE_243;
    memcpy(bytes + offset, &iter->value, sizeof(int64_t));
    offset += sizeof(int64_t);
  }

  return RC_OK;
}

retcode_t state_delta_deserialize(byte_t const *const bytes, size_t const size, state_delta_t *const delta) {
  uint64_t offset = 0;
  state_delta_entry_t *new = NULL;

  for (size_t i = 0; i < size / (FLEX_TRIT_SIZE_243 + sizeof(int64_t)); i++) {
    if ((new = malloc(sizeof(state_delta_entry_t))) == NULL) {
      return RC_OOM;
    }
    memcpy(new->hash, bytes + offset, FLEX_TRIT_SIZE_243);
    offset += FLEX_TRIT_SIZE_243;
    memcpy(&new->value, bytes + offset, sizeof(int64_t));
    offset += sizeof(int64_t);
    HASH_ADD(hh, *delta, hash, FLEX_TRIT_SIZE_243, new);
  }

  return RC_OK;
}

size_t state_delta_serialized_str_size(state_delta_t const delta) {
  if (delta == NULL) {
    return 0;
  }
  // For each line we persist the address followed by a ';' delimiter,
  // followed by the value (max is IOTA_SUPPLY which is 16 digits), followed by a new line
  return state_delta_size(delta) * (NUM_TRYTES_ADDRESS + 1 + 16 + 1);
}

retcode_t state_delta_serialize_str(state_delta_t const delta, char *const str) {
  state_delta_entry_t *iter = NULL, *tmp = NULL;
  uint32_t offset = 0;
  uint32_t value_len;

  HASH_ITER(hh, delta, iter, tmp) {
    if (flex_trits_to_trytes((tryte_t *)(str + offset), NUM_TRYTES_ADDRESS, iter->hash, NUM_TRITS_ADDRESS,
                             NUM_TRITS_ADDRESS) != NUM_TRITS_ADDRESS) {
      return RC_SNAPSHOT_STATE_DELTA_FAILED_DESERIALIZING;
    }
    offset += NUM_TRYTES_ADDRESS;
    str[offset] = ';';
    offset += 1;
    value_len = sprintf(str + offset, "%" PRId64 "\n", iter->value);
    offset += value_len;
  }

  str[offset] = '\0';

  return RC_OK;
}

retcode_t state_delta_deserialize_str(char const *const str, state_delta_t *const delta) {
  retcode_t ret = RC_OK;
  flex_trit_t curr_address[FLEX_TRIT_SIZE_243];
  int64_t value;
  char const *ptr = str;
  char *token;
  uint64_t supply = 0;

  token = strtok((char *)ptr, ";");
  if (token == NULL) {
    return RC_SNAPSHOT_INVALID_FILE;
  }

  while (token != NULL) {
    if (flex_trits_from_trytes(curr_address, NUM_TRITS_ADDRESS, (tryte_t *)token, NUM_TRYTES_ADDRESS,
                               NUM_TRYTES_ADDRESS) != NUM_TRYTES_ADDRESS) {
      return RC_SNAPSHOT_INVALID_FILE;
    }

    if ((token = strtok(NULL, "\n")) == NULL) {
      return RC_SNAPSHOT_INVALID_FILE;
    }

    if (sscanf(token, "%" PRId64 "", &value) != 1) {
      return RC_SNAPSHOT_INVALID_FILE;
    }

    if (value > 0) {
      supply += value;
    } else if (value < 0) {
      ret = RC_SNAPSHOT_INCONSISTENT_SNAPSHOT;
      goto done;
    }

    ERR_BIND_RETURN(state_delta_add(delta, curr_address, value), ret);
    token = strtok(NULL, ";");
  }

  if (supply != IOTA_SUPPLY) {
    ret = RC_SNAPSHOT_INVALID_SUPPLY;
    goto done;
  }

done:
  if (ret) {
    state_delta_destroy(delta);
  }
  return ret;
}
