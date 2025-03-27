#include "col.h"
#include <string.h>
#include <stdlib.h>

struct col_schema *col_schema_alloc() {
  return (struct col_schema*)calloc(1, COL_SCHEMA_SIZE);
}

void col_schema_free(struct col_schema *s) {
  if (s != NULL) {
    free(s);
  }

  return;
}

void col_schema_read(struct col_schema *s, const void *src) {
  memcpy(s, src, COL_SCHEMA_SIZE);
}

void col_schema_write(const struct col_schema *s, void *dest) {
  memcpy(dest, s, COL_SCHEMA_SIZE);
}

uint8_t col_serialize(const struct col *c, struct col_schema *s) {
  uint8_t at;

  if (c == NULL || s == NULL) {
    return 0;
  }

  at = 0;

  memcpy(s->data + at, &c->name, COL_NAME_BYTES);
  at += COL_NAME_BYTES;
  memcpy(s->data + at, &c->type, COL_TYPE_BYTES);
  at += COL_TYPE_BYTES;
  memcpy(s->data + at, &c->size, COL_SIZE_BYTES);
  at += COL_SIZE_BYTES;

  return at;
}

uint8_t col_parse(struct col *c, const struct col_schema *s) {
  uint8_t at;

  if (c == NULL || s == NULL) {
    return 0;
  }

  at = 0;

  memcpy(&c->name, s->data + at, COL_NAME_BYTES);
  at += COL_NAME_BYTES;
  memcpy(&c->type, s->data + at, COL_TYPE_BYTES);
  at += COL_TYPE_BYTES;
  memcpy(&c->size, s->data + at, COL_SIZE_BYTES);
  at += COL_SIZE_BYTES;

  return at;
}

struct col *col_new(col_t t, char *name, uint8_t size) {
  size_t min;
  struct col *c = (struct col*)calloc(1, sizeof(struct col));

  c->type = (uint8_t)t;
  c->size = size;

  min = strlen(name);
  if (min > COL_NAME_BYTES - 1) {
    min = COL_NAME_BYTES - 1;
  }

  memcpy(c->name, name, min);
  c->name[min] = '\0';

  return c;
}

void col_del(struct col *c) {
  if (c != NULL) {
    if (c->val != NULL) {
      free(c->val);
    }

    free(c);
    c = NULL;
  }

  return;
}

struct col *col_int_new(char *name, uint8_t size) {
  return col_new(COL_INT, name, size);
}

struct col *col_float_new(char *name, uint8_t size) {
  return col_new(COL_FLOAT, name, size);
}

struct col *col_str_new(char *name, uint8_t size) {
  return col_new(COL_STR, name, size);
}

struct col *col_blob_new(char *name) {
  return col_new(COL_BLOB, name, 0);
}

