#ifndef INC_CMD_H
#define INC_CMD_H

#include <stdio.h>
#include <pthread.h>

struct cmd {
	char *str;
	FILE *in;
	struct cmd_ix *ix;
	int size;
	int run;
	pthread_mutex_t mux;
};

struct cmd_ix {
	int (*get)(struct cmd*);
	void (*proc)(char*, size_t len);
};

int cmd_init(struct cmd *c, FILE *in, struct cmd_ix *ix, int size);

int cmd_init_mux(struct cmd *c, FILE *in, struct cmd_ix *ix, int size);

int cmd_del(struct cmd *c);

int cmd_del_mux(struct cmd *c);

void cmd_loop(struct cmd *c);

void cmd_loop_mux(struct cmd *c);

struct cmd *cmd_run(struct cmd *c);

struct cmd *cmd_run_mux(struct cmd *c);

struct cmd *cmd_stop(struct cmd *c);

struct cmd *cmd_stop_mux(struct cmd *c);

#endif

