#include "db.h"
#include "table.h"
#include "alg.h"

#include <string.h>
#include <stdlib.h>

int db_open(struct db *d, struct db_ix *ix, char *path)
{
  d->ix = ix;
  return ix->open(d, path);
}

int table_cmp(const void* a, const void* b)
{
  struct table *t1 = (struct table*)a, *t2 = (struct table*)b;
  return strcmp(t1->name, t2->name);
}

void db_table_init(struct db *d, char *tname, uint16_t size)
{
  d->tables = realloc(d->tables, (d->size + 1) * sizeof(struct table));
  table_init(d->tables + d->size, tname, size);
  ++d->size;

  heap_sort(d->tables, d->size, sizeof(struct table), table_cmp);
}

struct table *db_table_get(struct db *d, char *tname)
{
  struct table k;
  table_init(&k, tname, 0);
  return b_search(&k, d->tables, d->size, sizeof(struct table), table_cmp);
}

void db_table_free(struct db *d, char *tname)
{
  struct table *t = db_table_get(d, tname);

  if (t == NULL)
    return;

  --d->size;

  if (t != d->tables + d->size)
    *t = d->tables[d->size];
  
  d->tables = realloc(d->tables, (d->size) * sizeof(struct table));
}

