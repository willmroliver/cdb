#include "tpool.h"

#include <unistd.h>
#include <stdlib.h>

/*--- PRIVATE METHODS BEGIN ---*/

void *tpool_routine(void *arg); 

int tpool_job_pop(struct tpool *p, struct tjob *j);

void *tjob_proc(struct tjob *j);

/*--- PRIVATE METHODS END ---*/

void tpool_init(struct tpool *p) 
{
  int i;

  p->size = sysconf(_SC_NPROCESSORS_CONF);
  p->run = 1;
  p->pool = malloc(p->size * sizeof(pthread_t));
  p->jobs = ring_alloc(p->size, sizeof(struct tjob));

  pthread_cond_init(p->cond_job, NULL);

  for (i = 0; i < p->size; ++i) {
    pthread_create(p->pool + i, NULL, tpool_routine, p);
  }
}

void tpool_del(struct tpool *p) 
{
  int i;

  free(p->pool);
  ring_free(p->jobs);
  p->run = 0;

  pthread_cond_broadcast(p->cond_job);

  for (i = 0; i < p->size; ++i) {
    pthread_join(p->pool[i], NULL);
  }

  pthread_cond_destroy(p->cond_job);
}

int tpool_job_push(struct tpool *p, void *(*job)(void*), void *arg)
{
  int err;

  struct tjob j = {
    .proc=job,
    .arg=arg,
  };

  pthread_mutex_lock(p->job_mux);
  if ((err = ring_push(p->jobs, &j)) == 0)
    pthread_cond_signal(p->cond_job);
  pthread_mutex_unlock(p->job_mux);

  return err;
}

/*--- PRIVATE METHODS BEGIN ---*/

void *tpool_routine(void *arg) 
{
  struct tpool *p = (struct tpool*)arg;
  struct tjob j;

  while (1) {
    pthread_mutex_lock(p->job_mux);
    pthread_cond_wait(p->cond_job, p->job_mux);

    if (!p->run)
      break;

    if (tpool_job_pop(p, &j) != 0)
      continue;

    tjob_proc(&j);
  }

  return NULL;
}

int tpool_job_pop(struct tpool *p, struct tjob *j)
{
  int err;

  pthread_mutex_lock(p->job_mux);
  err = ring_pop(p->jobs, j);
  pthread_mutex_unlock(p->job_mux);
  
  return err;
}

void *tjob_proc(struct tjob *j) 
{
  return j->proc(j->arg);
}

/*--- PRIVATE METHODS END ---*/
