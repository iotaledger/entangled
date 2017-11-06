#include "../lib/ccurl.h"
#include "../lib/hash.h"
#include "../lib/pearl_diver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#ifndef _WIN32
#include <sys/select.h>
#else
#define STDIN_FILENO 0
#endif

#define TRYTE_LENGTH 2673

#define HINTS                                                                  \
  "### CCURL DAEMON ###\nUsage:\n\tccurld <MinWeightMagnitude> <path>\n"
static volatile bool running = true;
static void shutdown();
static char* defaultPath = "/tmp/pow";
static char* path;

int main(int argc, char* argv[]) {
  char *out = NULL, *str, *digest = NULL;
  long min_weight_magnitude;
  int fd;

  signal(SIGINT, shutdown);
  if (argc < 2) {
    fprintf(stderr, HINTS);
    return 1;
  }

  if (argc > 2) {
    fprintf(stderr, "Copying!.\n");
    path = (char*)malloc(sizeof(char) * strlen(argv[2]));
    memcpy(path, argv[2], sizeof(char) * strlen(argv[2]));
  } else {
    path = defaultPath;
  }

  int result = mkfifo(path, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
  if (result != -1) {
    fprintf(stderr, "Pipe opened.\n");
  } else {
    fprintf(stderr, "Could not open pipe.");
    fprintf(stderr, " %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  min_weight_magnitude = atol(argv[1]);

  if (min_weight_magnitude == 0) {
    fprintf(stderr, HINTS);
    exit(EXIT_FAILURE);
  }

  if ((str = getenv("CCURL_LOOP_COUNT"))) {
    ccurl_pow_set_loop_count(atol(str));
  }
  if ((str = getenv("CCURL_OFFSET"))) {
    ccurl_pow_set_offset(atol(str));
  }
  fprintf(stderr, "ccurl starting\n");
  ccurl_pow_init();
  while (running) {
    fd = open(path, O_RDONLY);
    if (fd < 0) {
      fprintf(stderr, "Error opening FIFO.\n");
      exit(EXIT_FAILURE);
    }
    char buf[TRYTE_LENGTH + 1] = {0};

    read(fd, buf, sizeof(buf));
    close(fd);
    if (running) {
      time_t start, end;
      start = time(0);
      out = ccurl_pow(buf, min_weight_magnitude);
      end = time(0);
      if (out != NULL) {
        fd = open(path, O_WRONLY);
        write(fd, out, strlen(out));
        close(fd);
        digest = ccurl_digest_transaction(out);
        fprintf(stderr, "%s: %f s\n", digest, difftime(end, start));

        free((void*) digest);
        free((void*) out);
      }
    }
  }
  if (unlink(path) < 0) {
    printf("Error erasing file.\n");
  } else {
    printf("%s erased.\n", path);
  }
  ccurl_pow_finalize();

  return 0;
}

static void shutdown() {
  fprintf(stderr, "\nCaught SIGINT. Shutting down... \n");
  running = false;
  ccurl_pow_interrupt();
  pid_t childPID;
  if ((childPID = fork()) < 0) {
    exit(EXIT_FAILURE);
  }
  if (!childPID) {
    int fd = open(path, O_WRONLY);
    char null[TRYTE_LENGTH] = {0};
    write(fd, null, strlen(null));
    close(fd);
  }

  if(path != defaultPath) {
    free((void*) path);
  }
}
