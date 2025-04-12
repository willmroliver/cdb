#include "tpool.h"

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

/*--- PRIVATE METHODS BEGIN ---*/

void *tpool_routine(void *arg);

/*--- PRIVATE METHODS END ---*/

void tpool_init(struct tpool *p, uint32_t buf_len) 
{
  int i;

  p->size = sysconf(_SC_NPROCESSORS_CONF);
  p->run = 1;
  p->pool = malloc(p->size * sizeof(pthread_t));
  p->jobs = ring_alloc(buf_len, sizeof(struct tjob));

  pthread_cond_init(&p->cond_job, NULL);
  pthread_mutex_init(&p->job_mux, NULL);

  for (i = 0; i < p->size; ++i) {
    pthread_create(p->pool + i, NULL, tpool_routine, p);
  }
}

void tpool_del(struct tpool *p) 
{
  int i;

  p->run = 0;
  
  pthread_cond_broadcast(&p->cond_job);

  for (i = 0; i < p->size; ++i) {
    pthread_join(p->pool[i], NULL);
  }

  pthread_cond_destroy(&p->cond_job);
  pthread_mutex_destroy(&p->job_mux);

  free(p->pool);
  ring_free(p->jobs);
}

int tpool_job_push(struct tpool *p, void *(*proc)(void*), void *arg)
{
  int err;

  struct tjob j = {
    .proc=proc,
    .arg=arg,
  };

  pthread_mutex_lock(&p->job_mux);
  if ((err = ring_push(p->jobs, &j)) == 0)
    pthread_cond_signal(&p->cond_job);
  pthread_mutex_unlock(&p->job_mux);

  return err;
}

/*--- PRIVATE METHODS BEGIN ---*/

void *tpool_routine(void *arg) 
{
  struct tpool *p = (struct tpool*)arg;
  struct tjob j;

  pthread_mutex_lock(&p->job_mux);

  while (1) {
    if (p->run && ring_empty(p->jobs))
      pthread_cond_wait(&p->cond_job, &p->job_mux);

    if (!p->run && ring_empty(p->jobs)) {
      pthread_mutex_unlock(&p->job_mux);
      return NULL;
    }

    if (ring_pop(p->jobs, &j) != 0)
      continue;

    pthread_mutex_unlock(&p->job_mux);
    j.proc(j.arg);
    pthread_mutex_lock(&p->job_mux);
  }

  pthread_mutex_unlock(&p->job_mux);
  return NULL;
}

/*--- PRIVATE METHODS END ---*/

