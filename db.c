#include "db.h"
#include "col.h"
#include "table.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

struct db* db_open(const char* path) {
  struct stat st = { 0 };
  DIR* dir;
  struct dirent* ent;
  struct table_schema* ts;
  struct col_schema* cs;
  uint16_t db_size, table_size;
  struct table* tables;
  struct col* cols;
  struct db* db;
  uint8_t min;

  if (stat(path, &st) == -1) {
    if (mkdir(path, 0700) == -1) {
      perror("failed to make db dir: ");
      return NULL;
    }
  }

  dir = opendir(path);
  if (dir == NULL) {
    return NULL;
  }

  ent = readdir(dir);
  ts = table_schema_alloc();
  cs = col_schema_alloc();

  // @todo - dynamically initialize the memory for all tables and their cols.
  while (ent != NULL) {
    FILE* f = fopen(ent->d_name, "r");
    if (f == NULL) {
      continue;
    }

    size_t len = 0;
    char* data = fgetln(f, &len);

    if (len != TABLE_SCHEMA_SIZE + 1) {
      continue;
    }

    table_schema_read(ts, data);
    // @todo - count tables parsed, realloc() if necessary, store and continue.

    while ((data = fgetln(f, &len)) && len == COL_SCHEMA_SIZE + 1) {
      col_schema_read(cs, data);
      // @todo - count cols parsed, realloc() if necessary, store and continue.
    } 

    // @todo - realloc cols to actual size.
  }

  // @todo - realloc tables to actual size (we probably doubled the alloc size each time, right?)

  table_schema_free(ts);
  col_schema_free(cs);

  db = (struct db*)calloc(1, sizeof(struct db));
  min = strlen(path);
  if (min >= 32) {
    min = 31;
  }

  memcpy(db->name, path, min);
  db->name[min] = '\0';

  db->size = db_size;
  db->tables = tables;

  return db;
}
