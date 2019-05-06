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
#include <stdlib.h>

#include "utils/files.h"
#include "utils/logger_helper.h"

bool iota_utils_file_exist(char const *const file_path) {
  if (access(file_path, F_OK) == -1) {
    return false;
  }
  return true;
}

retcode_t iota_utils_copy_file(const char *to, const char *from) {
  retcode_t ret = RC_OK;
  int fd_to, fd_from;
  char buf[4096];
  ssize_t nread;
  int saved_errno;

  fd_from = open(from, O_RDONLY);
  if (fd_from < 0) {
    return RC_UTILS_FAILED_OPEN_SRC_FILE;
  }

  fd_to = open(to, O_WRONLY | O_CREAT, 0666);
  if (fd_to < 0) {
    ret = RC_UTILS_FAILED_OPEN_CREATE_DST_FILE;
    goto out_error;
  }

  while (nread = read(fd_from, buf, sizeof buf), nread > 0) {
    char *out_ptr = buf;
    ssize_t nwritten;

    do {
      nwritten = write(fd_to, out_ptr, nread);

      if (nwritten >= 0) {
        nread -= nwritten;
        out_ptr += nwritten;
      } else if (errno != EINTR) {
        ret = RC_UTILS_FAILED_TO_COPY_FILE;
        goto out_error;
      }
    } while (nread > 0);
  }

  if (nread == 0) {
    if (close(fd_to) < 0) {
      fd_to = -1;
      ret = RC_UTILS_FAILED_CLOSE_FILE;
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
  return ret;
}

retcode_t iota_utils_remove_file(const char *file_path) {
  int status;

  status = remove(file_path);
  if (status) {
    return RC_UTILS_FAILED_REMOVE_FILE;
  }
  return RC_OK;
}

retcode_t iota_utils_overwrite_file(char const *const file_path, char const *const content) {
  retcode_t ret = RC_OK;
  FILE *file;
  size_t write_count = 0;
  size_t content_size = strlen(content_size);

  if ((file = fopen(file_path, "w"))) {
    write_count = fwrite(content, sizeof(char), strlen(content), file);
    if (write_count < content_size) {
      printf("LS: Failed to write file, errno: %d", errno);
      return RC_UTILS_FAILED_WRITE_FILE;
    }

  } else {
    return RC_UTILS_FAILED_TO_OPEN_FILE;
  }

  if (file) {
    fclose(file);
  }
  return ret;
}

retcode_t iota_utils_read_file_into_buffer(char const *const file_path, char **const buffer) {
  FILE *fp;
  long buffer_size;
  size_t offset = 0;

  if (!iota_utils_file_exist(file_path)) {
    return RC_UTILS_FILE_DOES_NOT_EXITS;
  }

  fp = fopen(file_path, "r");

  if (fp != NULL) {
    /* Go to the end of the file. */
    if (fseek(fp, 0L, SEEK_END) == 0) {
      /* Get the size of the file. */
      buffer_size = ftell(fp);
      if (buffer_size == -1) {
        return RC_UTILS_FAILED_READ_FILE;
      }

      /* Allocate our buffer to that size. */
      *buffer = malloc(sizeof(char) * (buffer_size + 1));

      /* Go back to the start of the file. */
      if (fseek(fp, 0L, SEEK_SET) != 0) {
        return RC_UTILS_FAILED_READ_FILE;
      }

      /* Read the entire file into memory. */
      while (offset < buffer_size) {
        offset += fread(*(buffer + offset), sizeof(char), buffer_size, fp);
        if (ferror(fp) != 0) {
          return RC_UTILS_FAILED_READ_FILE;
        }
      }

      (*buffer)[offset++] = '\0'; /* Just to be safe. */
    }
    fclose(fp);
  } else {
    return RC_UTILS_FAILED_TO_OPEN_FILE;
  }

  return RC_OK;
}
