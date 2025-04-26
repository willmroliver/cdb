#include "sched/fiber.h"

#include <stdio.h>

int main() 
{
	struct fiber f;
	f.rip = 10;
	f.stack = 11;
	
	printf(
		"%llu == %llu\n%llu == %llu\n", 
		f.rip, fiber_get_rip(&f), 
		f.stack, 
		fiber_get_stack(&f)
	);

	return 0;
}

