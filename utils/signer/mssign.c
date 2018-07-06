#include "common/curl-p/trit.h"
#include "common/kerl/hash.h"
#include "common/sign/v2/iss_curl.h"
#include "common/trinary/trit_tryte.h"
#include "mam/v1/merkle.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CHARGS "d:s:i:x:t:"

static struct option long_options[] = {
    {"message", required_argument, NULL, 'm'},
    {"seed", required_argument, NULL, 's'},
    {"index", required_argument, NULL, 'i'},
    {"sec-level", required_argument, NULL, 'x'},
    {"tree", required_argument, NULL, 't'},
    {NULL, 0, NULL, 0}};

typedef struct {
  int key_i;
  int64_t offset;
  size_t sec;
  trit_t *msg;
  size_t msg_len;
  trit_t seed[HASH_LENGTH];
  trit_t *tree;
  size_t tree_len;
  size_t n_leafs;
} ToSign;

int test_hash(trit_t *hash, size_t security) {
  size_t i;
  size_t sum = 0;

  for (i = 0; i < HASH_LENGTH / 3; i++) {
    sum += hash[i];
  }
  if (security == 1 && sum != 0) {
    return 1;
  }
  for (i = HASH_LENGTH / 3; i < 2 * HASH_LENGTH / 3; i++) {
    sum += hash[i];
  }
  if (security == 2 && sum != 0) {
    return 1;
  }
  for (i = 2 * HASH_LENGTH / 3; i < HASH_LENGTH; i++) {
    sum += hash[i];
  }
  return sum != 0;
}

int increment_trits(trit_t *t, size_t length) {
  trit_t *end = &t[length];
  trit_t c = 0;
  for (; t < end; t++) {
    (*t) += 1 + c;
    c = *t > 1;
    if (c) {
      *t = -1;
    } else {
      break;
    }
  }
  return t == end;
}

int find_nonce(ToSign *args, trit_t *hash) {
  trit_t nonce[HASH_LENGTH] = {0};
  tryte_t nonce_str[(HASH_LENGTH / 9) + 1] = {0};
  Kerl k, k_copy;
  init_kerl(&k);
  kerl_absorb(&k, args->msg, args->msg_len);
  memcpy(&k_copy, &k, sizeof(Kerl));
  do {
    memcpy(&k, &k_copy, sizeof(Kerl));
    increment_trits(nonce, HASH_LENGTH / 3);
    kerl_hash(nonce, HASH_LENGTH, hash, &k);
  } while (test_hash(hash, args->sec));
  trits_to_trytes(nonce, nonce_str, sizeof(nonce) / sizeof(trit_t));
  fprintf(stdout, "%s\n", nonce_str);
  return 0;
}

int leaf_count(size_t count, size_t depth, size_t acc) {
  size_t acc_next = acc + (1 << depth);
  if (acc_next >= count) {
    return 1 << depth;
  }
  return leaf_count(count, depth + 1, acc_next);
}

void print_siblings(ToSign *args) {
  size_t depth, sib_length;

  depth = merkle_depth(args->n_leafs);
  sib_length = (depth - 1) * HASH_LENGTH;

  trit_t sib_branch[sib_length];
  tryte_t siblings[((depth - 1) * HASH_LENGTH / 3) + 1];

  memset(sib_branch, 0, sizeof(sib_branch));
  memset(siblings, 0, sizeof(siblings));

  merkle_branch(args->tree, sib_branch, args->tree_len * HASH_LENGTH, depth,
                args->key_i, args->n_leafs);
  trits_to_trytes(sib_branch, siblings, sib_length);
  fprintf(stdout, "%s\n", siblings);
}

int merkle_sign(ToSign *args) {
  size_t key_length = ISS_KEY_LENGTH * args->sec;
  Curl c;
  c.type = CURL_P_81;
  init_curl(&c);
  {
    trit_t sigkey[key_length];
    tryte_t signature[(key_length / 3) + 1];
    trit_t hash[HASH_LENGTH] = {0};
    find_nonce(args, hash);

    memset(signature, 0, sizeof(signature));

    iss_curl_key(args->seed, sigkey, key_length, &c);
    curl_reset(&c);
    iss_curl_signature(sigkey, hash, 0, sigkey, key_length, &c);
    curl_reset(&c);
    trits_to_trytes(sigkey, signature, key_length);
    fprintf(stdout, "%s\n", signature);
  }
  print_siblings(args);
  return 0;
}

int main(int argc, char *argv[]) {
  char ch;
  size_t l;
  ToSign field = {0};
  while ((ch = getopt_long(argc, argv, CHARGS, long_options, NULL)) != -1) {
    // check to see if a single character or long option came through
    switch (ch) {
      case 'm':
        l = strlen(optarg);
        field.msg_len = l * 3;
        field.msg = calloc(field.msg_len, sizeof(trit_t));
        trytes_to_trits((tryte_t *)optarg, field.msg, l);
        break;
      case 's':
        trytes_to_trits((tryte_t *)optarg, field.seed,
                        strnlen(optarg, HASH_LENGTH / 3));
        break;
      case 't':
        l = strlen(optarg);
        field.tree_len = l * 3;
        field.n_leafs = leaf_count(field.tree_len / HASH_LENGTH, 0, 0);
        field.tree = calloc(field.tree_len, sizeof(trit_t));
        trytes_to_trits((tryte_t *)optarg, field.tree, l);
        break;
      case 'i':
        field.key_i = strtol(optarg, (char **)NULL, 10);
        break;
      case 'x':
        field.sec = strtol(optarg, (char **)NULL, 10);
        break;
      case 'o':
        field.offset = strtol(optarg, (char **)NULL, 10);
        break;
    }
  }
  if (field.sec == 0) {
    field.sec = 1;
  }
  if (field.key_i >= field.n_leafs) {
    printf("key index outside of range\n");
    exit(1);
  }
  return merkle_sign(&field);
}
