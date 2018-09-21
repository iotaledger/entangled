/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "types.h"

char_buffer_t* char_buffer_new() {
  char_buffer_t* out = (char_buffer_t*)malloc(sizeof(char_buffer_t));
  out->length = 0;
  out->data = NULL;
  return out;
}

retcode_t char_buffer_allocate(char_buffer_t* in, size_t n) {
  if (in->length != 0) {
    return RC_OK;
  }
  in->data = (char*)malloc(sizeof(char) * (n + 1));
  if (in->data == NULL) {
    return RC_CCLIENT_OOM;
  }
  in->length = n;
  *(in->data + n) = '\0';
  return RC_OK;
}

void char_buffer_free(char_buffer_t* in) {
  if (in) {
    if (in->data) {
      free(in->data);
    }
    free(in);
  }
}

int_array* int_array_new() {
  int_array* out = (int_array*)malloc(sizeof(int_array));
  out->size = 0;
  out->array = NULL;
  return out;
}

retcode_t int_array_allocate(int_array* in, size_t n) {
  if (in->size != 0) {
    return RC_OK;
  }
  in->array = (int*)malloc(sizeof(int) * (n + 1));
  if (in->array == NULL) {
    return RC_CCLIENT_OOM;
  }
  in->size = n;
  return RC_OK;
}

void int_array_free(int_array* in) {
  if (in) {
    if (in->array) {
      free(in->array);
    }
    free(in);
  }
}

int_array_array* int_array_array_new() {
  int_array_array* out = (int_array_array*)malloc(sizeof(int_array_array));
  out->size = 0;
  out->array = NULL;
  return out;
}

retcode_t int_array_array_allocate(int_array_array* in, size_t n) {
  if (in->size != 0) {
    return RC_OK;
  }
  in->array = (int_array*)malloc(sizeof(int_array) * (n + 1));
  if (in->array == NULL) {
    return RC_CCLIENT_OOM;
  }
  in->size = n;
  return RC_OK;
}

void int_array_array_free(int_array_array* in) {
  if (in) {
    for (int i = 0; i < in->size; i++) {
      int_array_free(in->array + i);
    }
    free(in);
  }
}

char* int_array_to_string(int_array* in) {
  int len = in->size;
  char* str = (char*)malloc(sizeof(char) * len);

  for (int i = 0; i < len; i++) {
    sprintf(str + i, "%d", *(in->array + i));
  }
  return str;
}

int_array* string_to_int_array(char* in) {
  int len = strlen(in);
  int_array* in_arr = int_array_new();
  int_array_allocate(in_arr, len);

  for (int i = 0; i < len; i++) {
    *(in_arr->array + i) = atoi(in + i);
  }
  return in_arr;
}

retcode_t flex_hash_to_trytes(trit_array_p hash, char* trytes) {
  size_t trits_len = 0;
  if (trytes == NULL) {
    return RC_CCLIENT_FLEX_TRITS;
  }

  trits_len = flex_trits_to_trytes(
      (signed char*)trytes, num_flex_trits_for_trits(hash->num_trits),
      hash->trits, hash->num_trits, hash->num_trits);

  if (trits_len == 0) {
    return RC_CCLIENT_FLEX_TRITS;
  }
  return RC_OK;
}

retcode_t trytes_to_flex_hash(trit_array_p hash, const char* trytes) {
  size_t str_len = strlen(trytes);
  size_t trits_len = str_len * 3;
  size_t ret_trytes = flex_trits_from_trytes(
      hash->trits, trits_len, (const tryte_t*)trytes, str_len, str_len);

  if (ret_trytes == 0) {
    trit_array_set_null(hash);
    return RC_CCLIENT_FLEX_TRITS;
  }

  return RC_OK;
}

flex_hash_array_t* flex_hash_array_new() { return NULL; }

flex_hash_array_t* flex_hash_array_append(flex_hash_array_t* head,
                                          const char* trytes) {
  flex_hash_array_t* elt =
      (flex_hash_array_t*)malloc(sizeof(flex_hash_array_t));

  elt->hash = trit_array_new_from_trytes((tryte_t*)trytes);
  if (elt->hash) {
    LL_APPEND(head, elt);
  } else {
    free(elt);
  }
  return head;
}

int flex_hash_array_count(flex_hash_array_t* head) {
  flex_hash_array_t* elt = NULL;
  int count = 0;
  LL_COUNT(head, elt, count);
  return count;
}

trit_array_p flex_hash_array_at(flex_hash_array_t* head, size_t index) {
  flex_hash_array_t* elt = NULL;
  int count = 0;
  LL_FOREACH(head, elt) {
    if (count == index) {
      return elt->hash;
    }
    count++;
  }
  return NULL;
}

void flex_hash_array_free(flex_hash_array_t* head) {
  flex_hash_array_t *elt, *tmp;
  LL_FOREACH_SAFE(head, elt, tmp) {
    LL_DELETE(head, elt);
    trit_array_free(elt->hash);
    free(elt);
  }
}
