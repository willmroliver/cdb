#include "db_fs.h"
#include "utils/fs.h"

#include <errno.h>
#include <sys/dirent.h>
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

  DIR *dir, *dirc;
  uint16_t ntables = 0, it, ic;
  struct dirent *ent;

  struct table_schema *ts;
  struct col_schema *cs;
  size_t nitems;
  char sbuf[COL_SCHEMA_SIZE > TABLE_SCHEMA_SIZE ? COL_SCHEMA_SIZE : TABLE_SCHEMA_SIZE];
  char fbuf[256];
  FILE *f;
  struct table *tables;
  uint16_t plen;

  if (db == NULL || db->ix == NULL) 
    return;

  ix = db->ix;
  fs = (struct db_fs*)ix;
  db_fs_init(ix);

  plen = strlen(path);
  if (plen + 1 > 256)
    return;

  memcpy(fs->path, path, plen + 1);

  for (size_t i = 1; i <= 31; ++i) {
    if (i < 31 && path[plen - i - 2] != '/')
      continue;

    if (plen <= i + 2)
      break;

    memcpy(db->name, path + plen - i - 1, i + 1);
    db->name[i + 1] = '\0';
    break;
  }

  if (mkdir_recursive(path) == -1)
    return;


  dir = opendir(path);
  if (dir == NULL) 
    return;

  dirc = opendir(path);

  while ((ent = readdir(dirc))) {
    if (ent->d_type == DT_REG)
      ++ntables;
  }

  tables = calloc(sizeof(struct table), ntables);

  ent = readdir(dir);
  ts = table_schema_alloc();
  cs = col_schema_alloc();

  it = -1;

  while ((ent = readdir(dir)) != NULL) {
    if (ent->d_type != DT_REG)
      continue;

    snprintf(fbuf, sizeof(fbuf), "%s/%s", path, ent->d_name);

    f = fopen(fbuf, "r");
    if (f == NULL)
      continue;

    nitems = fread(sbuf, TABLE_SCHEMA_SIZE + 1, 1, f);
    printf("nitems: %zu\n", nitems);
    if (nitems != 1) 
      continue;

    printf("found schema\n");

    ++it;
    table_schema_read(ts, sbuf);
    table_parse(tables + it, ts);

    tables[it].cols = calloc(tables[it].size, sizeof(struct col));
    ic = -1;

    while (fread(sbuf, COL_SCHEMA_SIZE + 1, 1, f) == 1) {
      ++ic;
      col_schema_read(cs, sbuf);
      col_parse(tables[it].cols + ic, cs);
    } 
  }

  table_schema_free(ts);
  col_schema_free(cs);

  db->size = ntables + 1;
  db->tables = tables;
}

void read(struct db *d, struct table* t, char* tname) {
  // @todo - read table schema from FS
}

void write(struct db *d, struct table* t, char* tname) {
  // @todo - write table schema to FS
}

