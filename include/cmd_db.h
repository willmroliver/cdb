#ifndef INC_CMD_DB_H
#define INC_CMD_DB_H

#include "cmd.h"

struct cmd_db {
  struct cmd_ix *ix;
  struct db *db;
  void (*handle)(struct cmd*);
};

void cmd_db_init(struct cmd_db* ix);

void cmd_db_handle(struct cmd* c);

#endif

