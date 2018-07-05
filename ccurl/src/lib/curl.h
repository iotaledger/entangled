
#ifndef CURL_H
#define CURL_H
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hash.h"

typedef struct {
  char state[STATE_LENGTH];
} curl_t;

EXPORT void init_curl(curl_t* ctx);

EXPORT void absorb(curl_t* ctx, char* const trits, int length);
EXPORT void squeeze(curl_t* ctx, char* const trits, int length);
EXPORT void reset(curl_t* ctx);

#endif
