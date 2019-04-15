/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#if !defined(_WIN32) && defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#elif defined(_WIN32)
#include <io.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <memory.h>
#include <stdbool.h>

#include "utils/files.h"

retcode_t copy_file(const char *to, const char *from) {
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

    return RC_OK;
  }

out_error:
  saved_errno = errno;

  close(fd_from);
  if (fd_to >= 0) close(fd_to);

  errno = saved_errno;
  return RC_UTILS_FAILED_TO_COPY_FILE;
}

retcode_t remove_file(const char *file_path) {
  int status;

  status = remove(file_path);
  if (status) {
    return RC_UTILS_FAILED_REMOVE_FILE;
  }
  return RC_OK;
}

retcode_t write_file(char const *const file_path, char const *const content) {
  retcode_t ret;
  FILE *file;
  char file_path_backup[256];
  bool file_already_exist = false;
  if ((file = fopen(file_path, "r"))) {
    strcat(file_path_backup, file_path);
    strcat(file_path_backup, ".backup");
    if (rename(file_path, file_path_backup) == -1) {
      return RC_UTILS_FAILED_WRITE_FILE;
    }
    fclose(file);
    file_already_exist = true;
  }

  if ((file = fopen(file_path, "w"))) {
    fputs(content, file);
  } else {
    if (file_already_exist && rename(file_path_backup, file_path) == -1) {
      return RC_UTILS_FAILED_WRITE_FILE;
    }
  }

  if (file_already_exist) {
    ERR_BIND_GOTO(remove_file(file_path_backup), ret, cleanup);
  }

cleanup:
  if (file) {
    fclose(file);
  }
}
