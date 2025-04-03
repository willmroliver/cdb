#include "cli.h"

#include <stddef.h>
#include <errno.h>

void cli_init(struct cli *c, FILE *stream, int (*handler)(struct cli *))
{
  if (c == NULL)
    return;

  c->cmd = NULL;
  c->len = 0;
  c->stream = stream;
  c->handler = handler;
} 

void cli_run(struct cli *c)
{
  if (c == NULL)
    return;

  while (1)
    if (cli_read(c) == 0)
      cli_handle(c);
}

int cli_read(struct cli *c)
{
  if (c == NULL)
    return -1;

  c->cmd = fgetln(c->stream, c->len);

  if (ferror(c->stream))
    return errno;

  if (c->cmd == NULL || *c->len == 0 || *c->cmd == '\n')
    return -1;

  return 0;
}

void cli_handle(struct cli *c)
{
  if (c == NULL)
    return;

  c->handler(c);
}
