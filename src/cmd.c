#include "cmd.h"

#include <stddef.h>
#include <errno.h>

int cmd_read(struct cmd *c);

void cmd_handle(struct cmd *c);

struct cmd *cmd_lock(struct cmd *c);

struct cmd *cmd_unlock(struct cmd *c);

int cmd_init(struct cmd *c, FILE *in, struct cmd_ix *ix)
{
  if (c == NULL || in == NULL || ix == NULL) 
    return -1;

  c->in = in;
  c->ix = ix;
  c->run = 0;

  return 0;
} 

int cmd_init_mux(struct cmd *c, FILE *in, struct cmd_ix *ix)
{
  int n;

  if ((n = cmd_init(c, in, ix)) != 0)
    return n;

  if (pthread_mutex_init(&c->mux, NULL))
    return -1;

  return 0;
}

int cmd_destroy_mux(struct cmd *c) {
  if (c == NULL)
    return 0;
  return pthread_mutex_destroy(&c->mux);
}

void cmd_loop(struct cmd *c)
{
  cmd_run(c);

  while (c->run)
    if (cmd_read(c) == 0)
      cmd_handle(c);
}

void cmd_loop_mux(struct cmd *c)
{
  if (cmd_run_mux(c) == NULL)
    return;

  while (1) {
    if (cmd_lock(c) == NULL || !c->run || cmd_unlock(c) == NULL)
      break;

    if (cmd_read(c) == 0)
      cmd_handle(c);
    else
      break;
  }
}

struct cmd *cmd_run(struct cmd *c)
{
  c->run = 1;
  return c;
}

struct cmd *cmd_run_mux(struct cmd *c) 
{
  return cmd_unlock(cmd_run(cmd_lock(c)));
}

struct cmd *cmd_stop(struct cmd *c)
{
  c->run = 0;
  return c;
}

struct cmd *cmd_stop_mux(struct cmd *c)
{
  return cmd_unlock(cmd_run(cmd_lock(c)));
}

int cmd_read(struct cmd *c)
{
  return c->ix->read(c);
}

void cmd_handle(struct cmd *c)
{
  c->ix->handle(c);
}

struct cmd *cmd_lock(struct cmd *c) {
  if (c == NULL || pthread_mutex_lock(&c->mux) != 0)
    return NULL;
  return c;
}

struct cmd *cmd_unlock(struct cmd *c) {
  if (c == NULL || pthread_mutex_unlock(&c->mux) != 0)
    return NULL;
  return c;
}
