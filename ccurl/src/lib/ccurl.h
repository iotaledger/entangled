#ifndef _CCURL_H_
#define _CCURL_H_

#ifndef EXPORT
#if defined(_WIN32) && !defined(__MINGW32__)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#endif
#if defined(_WIN32) && !defined(__MINGW32__)
#else
#include <unistd.h>
#endif

EXPORT int ccurl_pow_init();
EXPORT void ccurl_pow_interrupt();
EXPORT void ccurl_pow_finalize();
EXPORT void ccurl_pow_set_loop_count(size_t c);
EXPORT void ccurl_pow_set_offset(size_t o);
EXPORT char* ccurl_pow(char* trytes, int min_weight_magnitude);
EXPORT char* ccurl_digest_transaction(char* trytes);

#endif /* _CCURL_H_ */
