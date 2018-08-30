/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include "ciri/conf/usage.h"

void iota_usage() {
  int i = 0;

  fprintf(stderr, "cIRI usage:\n");
  while (ciri_arguments_g[i].desc) {
    fprintf(stderr, "--%-20s (-%c) ", ciri_arguments_g[i].name,
            ciri_arguments_g[i].val);
    if (ciri_arguments_g[i].has_arg == REQUIRED_ARG) {
      fprintf(stderr, " arg ");
    } else if (ciri_arguments_g[i].has_arg == OPTIONAL_ARG) {
      fprintf(stderr, "[arg]");
    } else {
      fprintf(stderr, "     ");
    }
    fprintf(stderr, " %s", ciri_arguments_g[i].desc);
    fprintf(stderr, "\n");
    i++;
  }
}
