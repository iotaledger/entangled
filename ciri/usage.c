/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include "ciri/usage.h"

void iota_usage() {
  int i = 0;

  fprintf(stderr, "cIRI usage:\n");
  while (cli_arguments_g[i].desc) {
    fprintf(stderr, "--%-46s ", cli_arguments_g[i].name);
    if (cli_arguments_g[i].val < CONF_START) {
      fprintf(stderr, "(-%c)", cli_arguments_g[i].val);
    } else {
      fprintf(stderr, "    ");
    }
    fprintf(stderr, " %s", cli_arguments_g[i].desc);
    fprintf(stderr, "\n");
    i++;
  }
}
