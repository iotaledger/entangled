/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef _WIN32
#include "utils/windows.h"
#elif MACOS
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

size_t system_cpu_available() {
#ifdef WIN32
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#elif MACOS
  int nm[2];
  size_t len = 4;
  uint32_t count = 0;

  nm[0] = CTL_HW;
  nm[1] = HW_AVAILCPU;
  sysctl(nm, 2, &count, &len, NULL, 0);

  if (count < 1) {
    nm[1] = HW_NCPU;
    sysctl(nm, 2, &count, &len, NULL, 0);
    if (count < 1) {
      count = 1;
    }
  }
  return count;
#else
  long count = sysconf(_SC_NPROCESSORS_ONLN);
  return count < 1 ? 1 : count;
#endif
}
