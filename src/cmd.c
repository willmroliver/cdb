#include "cmd.h"

#include <stddef.h>
#include <stdlib.h>
#include <memory.h>

/*--- PRIVATE METHODS BEGIN ---*/

int cmd_get(struct cmd *c);

void cmd_proc(void *arg);

struct cmd *cmd_lock(struct cmd *c);

struct cmd *cmd_unlock(struct cmd *c);

/*--- PRIVATE METHODS END ---*/

int cmd_init(struct cmd *c, FILE *in, struct cmd_ix *ix, int size)
{
  if (c == NULL || in == NULL || ix == NULL) 
    return -1;

  c->str = malloc(size);
  if (c->str == NULL)
    return -1;

  c->in = in;
  c->ix = ix;
  c->run = 0;

  return 0;
} 

int cmd_init_mux(struct cmd *c, FILE *in, struct cmd_ix *ix, int size)
{
  int n;

  if ((n = cmd_init(c, in, ix, size)) != 0)
    return n;

  if (pthread_mutex_init(&c->mux, NULL))
    return -1;

  return 0;
}

int cmd_del(struct cmd *c) {
  if (c == NULL || c->str == NULL)
    return -1;

  free(c->str);
  return 0;
}

int cmd_del_mux(struct cmd *c) {
  int del;

  if ((del = cmd_del(c)) != 0)
    return del;

  return pthread_mutex_destroy(&c->mux);
}

void cmd_loop(struct cmd *c)
{
  cmd_run(c);

  while (c->run)
    if (cmd_get(c) == 0)
      cmd_proc(c);
}

void cmd_loop_mux(struct cmd *c)
{
  int run;

  if (cmd_run_mux(c) == NULL)
    return;

  while (1) {
    if (cmd_lock(c) == NULL)
      break;
    
    run = c->run;

    if (cmd_unlock(c) == NULL)
      break;

    if (!run)
      break;

    if (cmd_get(c) == 0)
      /*@todo - pass into a pthread (we need to write a scheduler first)*/
      cmd_proc(c);
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

int cmd_get(struct cmd *c)
{
  return c->ix->get(c);
}

void cmd_proc(void *arg)
{
  struct cmd *c = (struct cmd*)arg;

  int size;
  char *cmd;

  if (cmd_lock(c) == NULL)
    return;

  size = c->size;
  cmd = memcpy(malloc(size), c->str, size);

  cmd_unlock(c);
  
  if (cmd == NULL)
    return;

  c->ix->proc(cmd, size);

  free(cmd);
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
