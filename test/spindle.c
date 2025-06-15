#include "spindle.h"
#include "promise.h"

#include <stdio.h>
#include <assert.h>

/* --- HELPERS --- */

struct test_arg {
	struct promise *p;
	int *sum;
	int n;
};

void *wait_job(void *arg);

void *resolve_job(void *arg);

/* --- INTEGRATION TESTS --- */

int spindle_do_loop_test();

int main()
{
	int passed =
		spindle_do_loop_test();

	printf("spindle tests %s\n", passed ? "passed" : "failed");
	return 0;
}

/* --- INTEGRATION TESTS --- */

int spindle_do_loop_test()
{
	struct spindle s;
	struct promise p;
	int n = 64, sum = 0, i;

	struct test_arg arg = {
		.p=&p,
		.sum=&sum,
		.n=n,
	};

	struct job rj = {
		.arg=&arg,
		.proc=resolve_job,
	}, wj = {
		.arg=&arg,
		.proc=wait_job,
	};

	promise_init(&p, sizeof(int), n - 1);

	spindle_init(&s, n);
	spindle_do(&s, wj);

	for (i = 1; i < n; ++i)
		spindle_do(&s, rj);

	spindle_loop(&s);

	for (i = 0; i < n - 1; ++i)
		if (*((int*)p.value + i) != n + i)
			return 0;

	return ring_size(s.idle) == (unsigned)n && 
		dl_list_empty(s.running);
}

/* --- HELPERS --- */

void *wait_job(void *arg)
{
	struct test_arg *a = *(void**)arg;
	
	promise_wait(a->p, fiber_self(arg));
	assert(2*(a->n - 1) == *a->sum);

	return arg;
}

void *resolve_job(void *arg)
{
	struct test_arg *a = *(void**)arg;

	*a->sum += 1;
	fiber_yield(fiber_self(arg));
	*a->sum += 1;

	promise_resolve(a->p, a->sum);
	return arg;
}

