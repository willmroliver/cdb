#include "table.h"
#include <string.h>
#include <stdlib.h>

struct table_schema *table_schema_alloc() {
  return (struct table_schema*)calloc(1, TABLE_SCHEMA_SIZE);
}

void table_schema_free(struct table_schema *s) {
  if (s != NULL) {
    free(s);
    s = NULL;
  }

  return;
}

void table_schema_read(struct table_schema *s, const void *src) {
  memcpy(s, src, TABLE_SCHEMA_SIZE);
}

void table_schema_write(const struct table_schema *s, void *dest) {
  memcpy(dest, s, TABLE_SCHEMA_SIZE);
}

uint8_t table_serialize(const struct table *t, struct table_schema *s) {
  uint8_t at;

  if (t == NULL || s == NULL) {
    return 0;
  }

  at = 0;
  memcpy(s->data + at, &t->name, TABLE_NAME_BYTES);
  at += TABLE_NAME_BYTES;
  memcpy(s->data + at, &t->size, TABLE_SIZE_BYTES);
  at += TABLE_SIZE_BYTES;

  return at;
}

uint8_t table_parse(struct table *t, const struct table_schema *s) {
  uint8_t at;

  if (t == NULL || s == NULL) {
    return 0;
  }

  at = 0;

  memcpy(&t->name, s->data + at, TABLE_NAME_BYTES);
  at += TABLE_NAME_BYTES;
  memcpy(&t->size, s->data + at, TABLE_SIZE_BYTES);
  at += TABLE_SIZE_BYTES;

  return at;
}

void table_init(struct table *t, char *name, uint16_t size) {
  uint8_t min;

  if (t == NULL) {
    return; 
  }

  t->size = size;
  t->cols = (struct col*)calloc(size, sizeof(struct col));

  min = strlen(name);
  if (min > TABLE_NAME_BYTES - 1) {
    min = TABLE_NAME_BYTES - 1; 
  }

  memcpy(t->name, name, min);
  t->name[min] = '\0';
}

void table_free(struct table *t) {
  if (t == NULL) {
    return;
  }

  if (t->size > 0) {
    for (int i = 0; i < t->size; ++i) {
      col_free(t->cols + i);
    }

    free(t->cols);
  }

  free(t);
}

