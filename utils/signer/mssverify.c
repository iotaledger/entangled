#include "common/curl-p/trit.h"
#include "common/kerl/hash.h"
#include "common/sign/v2/iss_curl.h"
#include "common/trinary/trit_tryte.h"
#include "mam/v1/merkle.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CHARGS "m:n:s:i:x:"

static struct option long_options[] = {
    {"message", required_argument, NULL, 'm'},
    {"nonce", required_argument, NULL, 'n'},
    {"signature", required_argument, NULL, 's'},
    {"index", required_argument, NULL, 'i'},
    {"siblings", required_argument, NULL, 'x'},
    {NULL, 0, NULL, 0}};

typedef struct {
  tryte_t *msg;
  size_t msg_len;
  tryte_t *nonce;
  size_t nonce_len;
  tryte_t *sig;
  size_t sig_len;
  tryte_t *sib;
  size_t sib_len;
  size_t i;
} ToVerify;

int merkle_verify(ToVerify *args) {
  trit_t hash[HASH_LENGTH];
  {
    Kerl k;
    init_kerl(&k);
    trit_t nonce[args->nonce_len * 3];
    trit_t msg[args->msg_len * 3];
    trytes_to_trits(args->msg, msg, args->msg_len);
    if (args->nonce) {
      trytes_to_trits(args->nonce, nonce, args->nonce_len);
      kerl_absorb(&k, msg, sizeof(msg) / sizeof(trit_t));
      kerl_hash(nonce, sizeof(nonce) / sizeof(trit_t), hash, &k);
    } else {
      kerl_hash(msg, sizeof(msg) / sizeof(trit_t), hash, &k);
    }
  }

  {
    Curl c;
    c.type = CURL_P_81;
    init_curl(&c);
    {
      trit_t sig[args->sig_len * 3];

      memset(sig, 0, sizeof(sig));

      trytes_to_trits(args->sig, sig, args->sig_len);
      iss_curl_sig_digest(hash, hash, 0, sig, args->sig_len * 3, &c);
      iss_curl_address(hash, hash, HASH_LENGTH, &c);
      curl_reset(&c);
    }
    {
      trit_t sib[args->sib_len * 3];
      tryte_t root[HASH_LENGTH / 3 + 1] = {0};
      trytes_to_trits(args->sib, sib, args->sib_len);

      printf("got this far nonco\n");
      merkle_root(hash, sib, sizeof(sib) / HASH_LENGTH, args->i, &c);
      trits_to_trytes(hash, root, HASH_LENGTH);
      fprintf(stdout, "%s\n", root);
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  char ch;
  ToVerify field = {0};
  while ((ch = getopt_long(argc, argv, CHARGS, long_options, NULL)) != -1) {
    // check to see if a single character or long option came through
    switch (ch) {
      case 'm':
        field.msg_len = strlen(optarg);
        field.msg = (tryte_t *)optarg;
        break;
      case 'n':
        field.nonce_len = strlen(optarg);
        field.nonce = (tryte_t *)optarg;
        break;
      case 's':
        field.sig_len = strlen(optarg);
        field.sig = (tryte_t *)optarg;
        break;
      case 'i':
        field.i = strtol(optarg, (char **)NULL, 10);
        break;
      case 'x':
        field.sib_len = strlen(optarg);
        field.sib = (tryte_t *)optarg;
        break;
    }
  }
  if (field.i >= (1 << (field.sib_len * 3 / HASH_LENGTH))) {
    fprintf(stderr, "signature index out of range\n");
    exit(1);
  }
  return merkle_verify(&field);
}
