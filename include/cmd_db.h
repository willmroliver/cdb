#ifndef INC_CMD_DB_H
#define INC_CMD_DB_H

#include "cmd.h"
#include <stddef.h>

struct cmd_db {
	struct cmd_ix ix;
	struct db *db;
};

void cmd_db_init(struct cmd_db* ix);

int cmd_db_get(struct cmd *c);

void cmd_db_proc(char *cmd, size_t len);

#endif

