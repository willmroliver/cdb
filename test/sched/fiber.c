#include "sched/fiber.h"
#include "job.h"

#include <stdio.h>
#include <string.h>


/* --- HELPERS --- */

void *simple_job(void*);

void *counter_job(void*);

/* --- INTEGRATION TESTS --- */

int fiber_run_yield_run_test();

int fiber_many_share_mem_test();

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
	char buf[32];

	struct fiber f = { .job={
		.proc=simple_job,
		.arg=buf,
	} };

	fiber_init(&f, 1024);

	fiber_run(&f);
	fiber_run(&f);

	return strcmp(buf, "Finishing job..") == 0;
}

int fiber_many_share_mem_test()
{
	int i, n = 10, c = 0;
	struct fiber f[n];
	struct job j = {
		.proc=counter_job,
		.arg=&c,
	};

	for (i = 0; i < n; ++i) {
		f[i].job = j;
		fiber_init(f + i, 1024);
		fiber_run(f + i);
	}

	for (i = 0; i < n; ++i)
		fiber_run(f + i);

	return c == 2*n;
}

/* --- HELPERS --- */

void *simple_job(void *arg) 
{
	char *buf = *(char**)arg; 

	snprintf(buf, 32, "Hello, Fiber!");
	fiber_yield(arg);
	snprintf(buf, 32, "%s", "Finishing job..");

	return arg;
}

void *counter_job(void *arg)
{
	int *c = *(int**)arg;
	*c = *c + 1;
	fiber_yield(arg);
	*c = *c + 1;
	return arg;
}
