#include "db.h"
#include "col.h"
#include "table.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

/*
 * @todo  we should really implement serialization for a db schema as well.
 *        then we could scrap the readdir() loop to determine allocation size.  
*/
struct db *db_open(const char *path) {
  struct stat st = { 0 };
  DIR *dir, *dirc;
  uint16_t ntables = 0, ncols = 0, it = -1, ic = -1;
  struct dirent *ent;
  struct table_schema *ts;
  struct col_schema *cs;
  size_t nitems;
  char buf[COL_SCHEMA_SIZE > TABLE_SCHEMA_SIZE ? COL_SCHEMA_SIZE : TABLE_SCHEMA_SIZE];
  FILE *f;
  struct table *tables;
  struct col *cols;
  struct db *db;
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

  dirc = dir;
  while (readdir(dirc)) {
    ++ntables;
  }

  tables = malloc(ntables * sizeof(struct table));

  ent = readdir(dir);
  ts = table_schema_alloc();
  cs = col_schema_alloc();

  while (ent != NULL) {
    f = fopen(ent->d_name, "r");
    if (f == NULL) {
      continue;
    }

    nitems = fread(buf, TABLE_SCHEMA_SIZE + 1, 1, f);
    if (nitems != 1) {
      continue;
    }

    ++it;
    tables[it] = *(struct table*)calloc(1, sizeof(struct table));
    table_schema_read(ts, buf);
    table_parse(tables + it, ts);

    tables[it].cols = (struct col*)calloc(tables[it].size, sizeof(struct col));

    while (fread(buf, COL_SCHEMA_SIZE + 1, 1, f) == 1) {
      ++ic;
      cols[ic] = *(struct col*)calloc(1, sizeof(struct col));
      col_schema_read(cs, buf);
      col_parse(cols + ic, cs);
    } 
  }

  table_schema_free(ts);
  col_schema_free(cs);

  db = (struct db*)calloc(1, sizeof(struct db));
  min = strlen(path);
  if (min >= 32) {
    min = 31;
  }

  memcpy(db->name, path, min);
  db->name[min] = '\0';

  db->size = ntables + 1;
  db->tables = tables;

  return db;
}

void db_table_new(struct db *d, char *tname, uint16_t size) {
  d->tables = realloc(d->tables, d->size + 1);
  d->tables[d->size] = *table_new(tname, size);
}

