#include "db_fs.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void db_fs_init(struct db_ix *ix) {
  ix->open = open;
  ix->read = read;
  ix->write = write;
}

void open(struct db *db, char *path) {
  struct db_ix *ix;
  struct db_fs *fs;

  struct stat st = { 0 };
  DIR *dir, *dirc;
  uint16_t ntables = 0, it = -1, ic = -1;
  struct dirent *ent;
  
  struct table_schema *ts;
  struct col_schema *cs;
  size_t nitems;
  char buf[COL_SCHEMA_SIZE > TABLE_SCHEMA_SIZE ? COL_SCHEMA_SIZE : TABLE_SCHEMA_SIZE];
  FILE *f;
  struct table *tables;
  uint16_t plen;

  if (db == NULL || db->ix == NULL) {
    return;
  }

  ix = db->ix;
  fs = (struct db_fs*)ix;
  db_fs_init(ix);

  plen = strlen(path);
  memcpy(fs->path, path, plen + 1);
  if (strlen(path) > PATH_MAX) {
    return;
  }

  if (stat(path, &st) == -1) {
    if (mkdir(path, 0700) == -1) {
      perror("failed to make db dir: ");
      return;
    }
  }

  dir = opendir(path);
  if (dir == NULL) {
    return;
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
    ic = -1;

    while (fread(buf, COL_SCHEMA_SIZE + 1, 1, f) == 1) {
      ++ic;
      tables[it].cols[ic] = *(struct col*)calloc(1, sizeof(struct col));
      col_schema_read(cs, buf);
      col_parse(tables[it].cols + ic, cs);
    } 
  }

  table_schema_free(ts);
  col_schema_free(cs);

  for (size_t i = 1; i <= 31; ++i) {
    if (i == 31 || path[plen - i - 2] == '/') { 
      memcpy(db->name, path + plen - i, i);
    }
  }

  db->size = ntables + 1;
  db->tables = tables;
}

void read(struct db *d, struct table* t, char* tname) {
  // @todo - read table schema from FS
}

void write(struct db *d, struct table* t, char* tname) {
  // @todo - write table schema to FS
}

