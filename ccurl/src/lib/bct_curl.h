

#ifndef BCT_CURL_H
#define BCT_CURL_H
#include "hash.h"

typedef struct { 
	bc_trit_t low[STATE_LENGTH]; 
	bc_trit_t hi[STATE_LENGTH]; 
} bct_curl_t;

/*
EXPORT void init_bct_curl(bct_curl_t* ctx);
EXPORT void absorb_bct(bct_curl_t* ctx, bc_trit_t* const trits, int length);
EXPORT void squeeze_bct(bct_curl_t* ctx, bc_trit_t* const trits, int length);
EXPORT void transform_bct(bct_curl_t* ctx);
EXPORT void reset_bct(bct_curl_t* ctx);
*/

#endif
