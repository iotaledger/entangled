#ifndef HASH_H
#define HASH_H

#ifndef EXPORT
#if defined(_WIN32)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#endif
#if defined(_WIN32) && !defined(MINGW)
#else
#include <unistd.h>
#endif

#include <stdint.h>
#define HASH_LENGTH 243
#define STATE_LENGTH 3 * HASH_LENGTH
#define TRYTE_LENGTH 2673
#define TRANSACTION_LENGTH TRYTE_LENGTH * 3
typedef int64_t bc_trit_t;
typedef char trit_t;

#ifndef DEBUG
#define DEBUG
#endif // DEBUG
#endif
