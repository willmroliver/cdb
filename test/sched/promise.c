#include "sched/promise.h"
#include "fiber.h"

#include <assert.h>
#include <stdio.h>

/* --- HELPERS --- */

#define N 100

void *resolve_job(void *arg);

void *wait_job(void *arg);

/* --- INTEGRATION TESTS --- */

int promise_wait_resolve_test(int);

int main()
{
	int passed =
		promise_wait_resolve_test(1) &&
		promise_wait_resolve_test(100);

	printf("promise tests %s\n", passed ? "passed" : "failed");
	return 0;
}

/* --- INTEGRATION TESTS --- */

int promise_wait_resolve_test(int n)
{
	int i;
	fiber_t f[n + 1];
	struct promise p;
	struct job rj = {
		.proc=resolve_job,
		.arg=&p, 
	}, wj = {
		.proc=wait_job,
		.arg=&p,
	};

	fiber_init(f, 256);
	fiber_do(f, wj);
	promise_init(&p, sizeof(int), n);

	for (i = 1; i <= n; ++i) {
		/* confirm fiber is waiting */
		fiber_run(f);
		assert(fiber_flags_test(f, FIBER_RUNNING | FIBER_YIELDED));
		assert(!fiber_flags_test(f, FIBER_DONE));

		fiber_init(f + i, 4096);
		fiber_do(f + i, rj);
		fiber_run(f + i);
	}

	fiber_run(f);
	assert(fiber_flags_test(f, FIBER_DONE));

	return 1;
}

/* --- HELPERS --- */

void *resolve_job(void *arg) 
{
	int ans = N;
	promise_resolve(*(struct promise**)arg, &ans);
	return arg;
}

void *wait_job(void *arg)
{
	size_t i;
	struct promise *p = *(struct promise**)arg;
	int *ans = promise_wait(p, fiber_self(arg));

	for (i = 0; i < p->count; ++i)
		assert(ans[i] == N);

	return arg;
}

