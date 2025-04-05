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

void db_fs_init(struct db_fs *fs)
{
  fs->ix->open = db_fs_open;
  fs->ix->table_open = db_fs_table_open;
  fs->ix->table_save = db_fs_table_save;
}

int db_fs_open(struct db *d, char *path)
{
  struct db_ix *ix;
  struct db_fs *fs;
  uint16_t plen;
  size_t i;
  DIR *dir;
  uint16_t ntables = 0;
  struct dirent *ent;
  
  if (d == NULL || d->ix == NULL) 
    return -1;

  ix = d->ix;
  fs = (struct db_fs*)ix;
  db_fs_init(fs);

  if ((plen = strlen(path)) > 255)
    return -1;

  memcpy(fs->path, path, plen + 1);

  for (i = 1; i <= 31; ++i) {
    if (i < 31 && path[plen - i - 2] != '/')
      continue;

    if (plen <= i + 2)
      break;

    memcpy(d->name, path + plen - i - 1, i + 1);
    d->name[i + 1] = '\0';
    break;
  }

  if (mkdir_recursive(path) == -1)
    return -1;

  if ((dir = opendir(path)) == NULL) 
    return -1;

  while ((ent = readdir(dir)) != NULL) 
    if (ent->d_type == DT_REG)
      ++ntables;
  
  d->tables = calloc(sizeof(struct table), ntables);
  d->size = 0;
  rewinddir(dir);

  while ((ent = readdir(dir)) != NULL) {
    if (ent->d_type != DT_REG)
      continue;

    if (db_fs_table_open(d, d->tables + d->size, ent->d_name) == 0)
      ++d->size;
  }

  return 0;
}

int db_fs_table_open(struct db *d, struct table* t, char* tname)
{
  FILE *f = NULL;
  char fbuf[256];
  char sbuf[COL_SCHEMA_SIZE > TABLE_SCHEMA_SIZE ? COL_SCHEMA_SIZE : TABLE_SCHEMA_SIZE];
  uint16_t ncols = 0;
  struct table_schema ts = { 0 };
  struct col_schema cs = { 0 };

  if (d == NULL || t == NULL || tname == NULL)
    return -1;

  snprintf(fbuf, sizeof(fbuf), "%s/%s", ((struct db_fs*)d->ix)->path, tname);
  f = fopen(fbuf, "rb");
  if (f == NULL)
    return -1;

  if (table_schema_fread(&ts, f) != 0)
    return -1;

  table_parse(t, &ts);

  t->cols = calloc(t->size, sizeof(struct col));

  while (ncols < t->size && fread(sbuf, COL_SCHEMA_SIZE + 1, 1, f) == 1) {
    col_schema_read(&cs, sbuf);
    col_parse(t->cols + ncols, &cs);
    ++ncols;
  }

  t->size = ncols;
  fclose(f);

  return 0;
}

int db_fs_table_save(struct db *d, struct table* t, char* tname)
{
  char fbuf[256];
  FILE *f = NULL;
  struct table_schema ts = { 0 };
  struct col_schema cs = { 0 };
  uint16_t i;

  if (d == NULL || t == NULL || tname == NULL)
    return -1;

  snprintf(fbuf, sizeof(fbuf), "%s/%s", ((struct db_fs*)d->ix)->path, tname);
  if ((f = fopen(fbuf, "wb")) == NULL)
    return -1;

  table_serialize(t, &ts);
  if (table_schema_fwrite(&ts, f) != 0) 
    return -1;

  for (i = 0; i < t->size; ++i) {
    col_serialize(t->cols + i, &cs);
    if (col_schema_fwrite(&cs, f) != 0)
      return -1;
  }

  return 0;
}

