#include "sched/fiber.h"
#include "job.h"

#include <stdio.h>

void *job(void*);

int main() 
{
	struct fiber f;
	fiber_init(&f, 1024);
	f.job.proc = job; 
	f.job.arg = "Hello, Fiber!";

	fiber_run(&f);
	fiber_run(&f);
	
	return 0;
}

void *job(void *arg) 
{
	printf("%s\n", *(char**)arg);
	fiber_yield(arg);
	printf("Back again?\n");
	return arg;
}
