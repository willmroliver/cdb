#ifndef INC_CMD_H
#define INC_CMD_H

#include "table.h"
#include <sys/stat.h>

struct db {
  char name[32];
  uint16_t size;
  struct table* tables;
};

struct db* db_open(const char* path);

#endif
