#ifndef INC_DB_FS_H
#define INC_DB_FS_H

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#include "db.h"

#include <stddef.h>

struct db_fs {
  struct db_ix *ix;
  char path[PATH_MAX];
  char* tables;
};

void db_fs_init(struct db_ix*);

void open(struct db*, char*);

void read(struct db*, struct table*, char*);

void write(struct db*, struct table*, char*);

#endif

