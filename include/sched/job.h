#ifndef INC_SCHED_JOB_H
#define INC_SCHED_JOB_H

struct __attribute__((packed)) job {
	void *(*proc)(void*);
	void *arg;
};

#endif

