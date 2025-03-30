#ifndef INC_COL_H
#define INC_COL_H

#include <stdint.h>

#define COL_SCHEMA_SIZE 64
#define COL_NAME_BYTES 32
#define COL_TYPE_BYTES 1
#define COL_SIZE_BYTES 1

typedef enum {
  COL_INT,
  COL_FLOAT,
  COL_STR,
  COL_BLOB
} col_t;

struct col {
  char name[32];
  uint8_t type;
  uint8_t size;
  void *val;
};

struct col_schema {
  char data[COL_SCHEMA_SIZE];
};

struct col_schema *col_schema_alloc();

void col_schema_free(struct col_schema *s);

void col_schema_read(struct col_schema *s, const void *src);

void col_schema_write(const struct col_schema *s, void *dest);

uint8_t col_serialize(const struct col *c, struct col_schema *buf);

uint8_t col_parse(struct col *c, const struct col_schema *buf);

struct col *col_new(col_t t, char *name, uint8_t size);

void col_del(struct col *c);

struct col *col_int_new(char *name, uint8_t size);

struct col *col_float_new(char *name, uint8_t size);

struct col *col_str_new(char *name, uint8_t size);

struct col *col_blob_new(char *name);

#endif
