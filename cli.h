#ifndef INC_CLI_H
#define INC_CLI_H

#include <stdio.h>

struct cli {
  char *cmd;
  size_t *len;
  FILE *stream;
  int (*handler)(struct cli *c);
};

void cli_init(struct cli *c, FILE *stream,  int (*handler)(struct cli *c));

void cli_run(struct cli *c);

int cli_read(struct cli *c);

void cli_handle(struct cli *c);

#endif
