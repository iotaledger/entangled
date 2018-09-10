#include "signal.h"

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))

void signal_handler_posix(int signo) {
  if (signo == ctrl_c_signal) {
    end_core(ctrl_c);
  }
}

#elif defined(_WIN32)

BOOL signal_handler_WIN(DWORD dwCtrlType) {
  if (dwCtrlType == ctrl_c_signal) {
    end_core(ctrl_c);
    return TRUE;
  }
  return signal_error;
}

#endif
