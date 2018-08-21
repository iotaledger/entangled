/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#elif defined(_WIN32)
#include <io.h>
#endif

#include <errno.h>
#include <fcntl.h>

#include "utils/files.h"

int copy_file(const char *to, const char *from) {
  int fd_to, fd_from;
  char buf[4096];
  ssize_t nread;
  int saved_errno;

  fd_from = open(from, O_RDONLY);
  if (fd_from < 0) return -1;

  fd_to = open(to, O_WRONLY | O_CREAT, 0666);
  if (fd_to < 0) goto out_error;

  while (nread = read(fd_from, buf, sizeof buf), nread > 0) {
    char *out_ptr = buf;
    ssize_t nwritten;

    do {
      nwritten = write(fd_to, out_ptr, nread);

      if (nwritten >= 0) {
        nread -= nwritten;
        out_ptr += nwritten;
      } else if (errno != EINTR) {
        goto out_error;
      }
    } while (nread > 0);
  }

  if (nread == 0) {
    if (close(fd_to) < 0) {
      fd_to = -1;
      goto out_error;
    }
    close(fd_from);

    /* Success! */
    return 0;
  }

out_error:
  saved_errno = errno;

  close(fd_from);
  if (fd_to >= 0) close(fd_to);

  errno = saved_errno;
  return -1;
}
