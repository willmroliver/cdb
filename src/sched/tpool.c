#include "sched/tpool.h"
#include "sched/job.h"

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
	p->jobs = ring_alloc(buf_len, sizeof(struct job));

	pthread_cond_init(&p->cond_job, NULL);
	pthread_mutex_init(&p->job_mux, NULL);

	for (i = 0; i < p->size; ++i)
		pthread_create(p->pool + i, NULL, tpool_routine, p);
}

void tpool_del(struct tpool *p) 
{
	int i;

	pthread_mutex_lock(&p->job_mux);
	p->run = 0;
	pthread_mutex_unlock(&p->job_mux);

	pthread_cond_broadcast(&p->cond_job);

	for (i = 0; i < p->size; ++i)
		pthread_join(p->pool[i], NULL);

	pthread_cond_destroy(&p->cond_job);
	pthread_mutex_destroy(&p->job_mux);

	free(p->pool);
	ring_free(p->jobs);
}

int tpool_job_push(struct tpool *p, struct job j)
{
	int err;

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
	struct job j;

	/* It looks like we could move this to the top of the while block, */
	/* and remove the lock at the bottom, but this prevents us from */
	/* unconditionally unlocking as we exit... */
	pthread_mutex_lock(&p->job_mux);

	while (1) {
		if (ring_empty(p->jobs) && p->run)
			pthread_cond_wait(&p->cond_job, &p->job_mux);

		if (ring_empty(p->jobs) && !p->run)
			goto out;

		if (ring_pop(p->jobs, &j) != 0)
			continue;

		pthread_mutex_unlock(&p->job_mux);
		j.proc(j.arg);

		pthread_mutex_lock(&p->job_mux);
	}

out:
	pthread_mutex_unlock(&p->job_mux);
	return NULL;
}

/*--- PRIVATE METHODS END ---*/

