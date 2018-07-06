#include "iss_curl.h"
#include "common/curl-p/trit.h"

#define HASH_PREFIX curl
#define HASH_STATE Curl

#include "iss.c.inc"

#undef HASH_PREFIX
#undef HASH_STATE
