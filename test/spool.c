#include "spool.h"

#include <stdio.h>
#include <assert.h>

/* --- HELPERS --- */

void *try_job(void *arg);

void *wait_job(void *arg);

void *inc_job(void *arg);

/* --- INTEGRATION TESTS --- */

int spool_try_release_test();

int spool_try_contested_test();

int spool_wait_release_test();

int spool_inc_dec_test();

int main()
{
	int passed =
		spool_try_release_test() &&
		spool_try_contested_test() &&
		spool_wait_release_test() &&
		spool_inc_dec_test();

	printf("spool tests %s\n", passed ? "passed" : "failed");
	return 0;
}

/* --- INTEGRATION TESTS --- */

int spool_try_release_test()
{
	fiber_t f;
	spool_t s;
	struct job j = {
		.proc=try_job,
		.arg=&s,
	};

	fiber_init(&f, 1024);
	spool_init(&s);
	fiber_do(&f, j);

	/* acquire */
	fiber_run(&f);
	assert(spool_bearer(&s) == &f);

	/* release */
	fiber_run(&f);
	assert(spool_bearer(&s) == NULL);
	assert(spool_release(&s, &f) == -1);

	return 1;
}

int spool_try_contested_test()
{
	fiber_t f0, f1;
	spool_t s;
	struct job j = {
		.proc=try_job,
		.arg=&s,
	};

	fiber_init(&f0, 1024);
	spool_init(&s);
	fiber_do(&f0, j);

	/* acquire */
	fiber_run(&f0);

	/* spool unavailable */
	assert(spool_try(&s, &f1) == -1);

	/* release */
	fiber_run(&f0);

	/* available to second fiber */
	assert(spool_try(&s, &f1) == 0);

	return 1;
}

int spool_wait_release_test()
{
	fiber_t f0, f1;
	spool_t s;
	struct job j = {
		.proc=wait_job,
		.arg=&s,
	};
	int i;

	fiber_init(&f0, 1024);
	fiber_init(&f1, 1024);
	spool_init(&s);
	fiber_do(&f0, j);
	fiber_do(&f1, j);

	/* acquire */
	fiber_run(&f0);
	assert(spool_bearer(&s) == &f0);

	fiber_run(&f1);
	assert(spool_bearer(&s) == &f0);

	/* f1 waits */
	for (i = 0; i < 100; ++i)
		fiber_run(&f1);
	assert(spool_bearer(&s) == &f0);

	/* bearing fiber releases */
	assert(spool_release(&s, &f1) == -1);
	assert(spool_release(&s, &f0) == 0);

	/* f1 acquires */
	fiber_run(&f1);
	assert(spool_bearer(&s) == &f1);

	/* f1 completes job */
	fiber_run(&f1);
	assert(spool_bearer(&s) == NULL);

	return 1;
}

int spool_inc_dec_test()
{
	int n = 100, i;
	fiber_t f[n];
	spool_t wg;
	struct job inc_j = {
		.proc=inc_job,
		.arg=&wg,
	}, wait_j = {
		.proc=wait_job,
		.arg=&wg,
	};

	spool_init(&wg);

	/* we'll test inc/dec with a simple wait group impl. */
	for (i = 1; i < n; ++i) {
		fiber_init(f + i, 1024);
		fiber_do(f + i, inc_j);
		fiber_run(f + i);
	}

	fiber_init(f, 1024);
	fiber_do(f, wait_j);

	for (i = 1; i < n; ++i) {
		fiber_run(f);
		assert(spool_bearer(&wg) != f);
		fiber_run(f + i);
	}

	fiber_run(f);
	assert(spool_bearer(&wg) == f);
	fiber_run(f);
	assert(spool_bearer(&wg) == NULL);

	return 1;
}

/* --- HELPERS --- */

void *try_job(void *arg)
{
	fiber_t *f = fiber_self(arg);
	spool_t *s = *(spool_t**)arg;

	assert(spool_try(s, f) == 0);
	fiber_yield(f);
	assert(spool_release(s, f) == 0);

	return arg; 
}

void *wait_job(void *arg)
{
	fiber_t *f = fiber_self(arg);
	spool_t *s = *(spool_t**)arg;

	assert(spool_wait(s, f) == 0);
	fiber_yield(f);
	assert(spool_release(s, f) == 0);

	return arg;
}

void *inc_job(void *arg)
{
	fiber_t *f = fiber_self(arg);
	spool_t *s = *(spool_t**)arg;

	spool_inc(s);
	fiber_yield(f);
	spool_dec(s);

	return arg;
}
