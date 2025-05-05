#include "sched/fiber.h"
#include "sched/job.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* --- HELPERS --- */

void *simple_job(void*);

void *counter_job(void*);

/* --- INTEGRATION TESTS --- */

int fiber_run_yield_run_test();

int fiber_many_share_mem_test();

int fiber_recurring_test();

int main() 
{
	int passed =
		fiber_run_yield_run_test() &&
		fiber_many_share_mem_test();

	printf(passed ? "fiber tests passed\n" : "fiber tests failed\n");

	return 0;
}

/* --- INTEGRATION TESTS --- */

int fiber_run_yield_run_test()
{
	fiber_t f;
	char buf[32];
	struct job j = {
		.proc=simple_job,
		.arg=buf,
	};

	fiber_init(&f, 1024);
	fiber_do(&f, j);

	fiber_run(&f);
	assert(fiber_flags_test(&f, FIBER_YIELDED | FIBER_RUNNING | FIBER_READY));
	fiber_run(&f);
	assert(fiber_flags_test(&f, FIBER_DONE));
	assert(!fiber_flags_test(&f, FIBER_RUNNING));

	return strcmp(buf, "Finishing job..") == 0;
}

int fiber_many_share_mem_test()
{
	int i, n = 10, c = 0;
	fiber_t f[n];
	struct job j = {
		.proc=counter_job,
		.arg=&c,
	};

	for (i = 0; i < n; ++i) {
		fiber_init(f + i, 1024);
		fiber_do(f + i, j);
		fiber_run(f + i);
	}

	for (i = 0; i < n; ++i)
		fiber_run(f + i);
	
	return c == 2*n;
}

int fiber_recurring_test()
{
	int i, n = 10, c = 0;
	fiber_t f;
	struct job j = {
		.proc=counter_job,
		.arg=&c,
	};

	fiber_init(&f, 1024);
	fiber_do(&f, j);
	fiber_set_recurring(&f, 1);

	for (i = 0; i < n - 1; ++i) {
		fiber_run(&f);
		fiber_run(&f);
		assert(!fiber_flags_test(&f, FIBER_DONE));
	}

	fiber_set_recurring(&f, 0);
	fiber_run(&f);
	fiber_run(&f);
	assert(fiber_flags_test(&f, FIBER_DONE));

	/* once done, does nothing */
	for (i = 0; i < n; ++i)
		fiber_run(&f);
	assert(c == 10);

	return 1;
}

/* --- HELPERS --- */

void *simple_job(void *arg) 
{
	fiber_t *f = fiber_self(arg);
	char *buf = *(char**)arg; 

	snprintf(buf, 32, "Hello, Fiber!");
	fiber_yield(f);
	snprintf(buf, 32, "%s", "Finishing job..");

	return arg;
}

void *counter_job(void *arg)
{
	fiber_t *f = fiber_self(arg);
	int *c = *(int**)arg;

	*c = *c + 1;
	fiber_yield(f);
	*c = *c + 1;
	return arg;
}
