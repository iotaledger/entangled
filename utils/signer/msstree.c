#include "common/curl-p/trit.h"
#include "common/sign/v2/iss_curl.h"
#include "common/trinary/trit_tryte.h"
#include "mam/v1/merkle.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CHARGS "s:l:x:o:"

static struct option long_options[] = {
    {"seed", required_argument, NULL, 's'},
    {"leaf-count", required_argument, NULL, 'l'},
    {"sec-level", required_argument, NULL, 'x'},
    {"offset", required_argument, NULL, 'o'},
    {NULL, 0, NULL, 0}};

typedef struct {
  int n_key;
  int key_i;
  int64_t offset;
  size_t sec;
  trit_t seed[HASH_LENGTH];
} ToSign;

int main(int argc, char *argv[]) {
  char ch;
  ToSign args = {0};
  while ((ch = getopt_long(argc, argv, CHARGS, long_options, NULL)) != -1) {
    // check to see if a single character or long option came through
    switch (ch) {
      case 's':
        trytes_to_trits((tryte_t *)optarg, args.seed,
                        strnlen(optarg, HASH_LENGTH / 3));
        break;
      case 'l':
        args.n_key = strtol(optarg, (char **)NULL, 10);
        break;
      case 'x':
        args.sec = strtol(optarg, (char **)NULL, 10);
        break;
      case 'o':
        args.offset = strtol(optarg, (char **)NULL, 10);
        break;
    }
  }
  if (args.sec == 0) {
    args.sec = 1;
  }
  Curl c;
  size_t size = merkle_size(args.n_key);
  trit_t tree[HASH_LENGTH * size];
  tryte_t tree_trytes[HASH_LENGTH * size / 3 + 1];

  c.type = CURL_P_81;
  init_curl(&c);

  memset(tree, 0, sizeof(tree));
  memset(tree_trytes, 0, sizeof(tree_trytes));

  merkle_create(tree, args.n_key, args.seed, args.offset, args.sec, &c);
  trits_to_trytes(tree, tree_trytes, sizeof(tree) / sizeof(trit_t));
  fprintf(stdout, "%s\n", tree_trytes);

  return 0;
}
