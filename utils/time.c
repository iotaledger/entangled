/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__) || defined(__XTENSA__))
#include <sys/time.h>
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>  // for nanosleep
#else
#include <unistd.h>  // for usleep
#endif
#elif defined(_WIN32)
#include <windows.h>
#endif

#include "utils/time.h"

uint64_t current_timestamp_ms() {
#ifdef _WIN32
  SYSTEMTIME time;

  GetSystemTime(&time);
  LONG ms = (time.wSecond * 1000) + time.wMilliseconds;
  return ms;
#else
  struct timeval tv = {0};

  gettimeofday(&tv, NULL);
  uint64_t ms = tv.tv_sec * 1000ULL + tv.tv_usec / 1000ULL;
  return ms;
#endif
}

void sleep_ms(uint64_t milliseconds) {
#ifdef _WIN32
  Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
  struct timespec ts;
  ts.tv_sec = milliseconds / 1000;
  ts.tv_nsec = (milliseconds % 1000) * 1000000;
  nanosleep(&ts, NULL);
#else
  usleep(milliseconds * 1000);
#endif
}
