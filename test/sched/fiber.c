#include "sched/fiber.h"
#include "job.h"

#include <stdio.h>

void *job(void*);

int main() 
{
	struct fiber f;
	struct job j = {
		.proc=job,
		.arg=NULL,
	};

	fiber_init(&f, 1024);
	printf("stack size: %lu\n", f.size);
	fiber_run(&f, j);
	
	return 0;
}

void *job(void* arg) {
	printf("Hello, Fiber!\n");
	return arg;
}
