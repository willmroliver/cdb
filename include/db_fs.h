#ifndef INC_DB_FS_H
#define INC_DB_FS_H

#include "db.h"

#include <stddef.h>

struct db_fs {
  struct db_ix *ix;
  char path[256];
  char* tables;
};

void db_fs_init(struct db_ix*);

int open(struct db*, char*);

int read(struct db*, struct table*, char*);

int write(struct db*, struct table*, char*);

#endif

