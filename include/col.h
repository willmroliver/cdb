#ifndef INC_COL_H
#define INC_COL_H

#include <stdint.h>
#include <stdio.h>

#define COL_SCHEMA_SIZE 64
#define COL_NAME_BYTES 32
#define COL_TYPE_BYTES 1
#define COL_SIZE_BYTES 1

typedef enum {
  COL_INT = 1,
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

int col_schema_fread(struct col_schema *s, FILE *f);

int col_schema_fwrite(const struct col_schema *s, FILE *f);

uint8_t col_serialize(const struct col *c, struct col_schema *buf);

uint8_t col_parse(struct col *c, const struct col_schema *buf);

void col_init(struct col *c, col_t t, char *name, uint8_t size);

void col_free(struct col *c);

void col_int_init(struct col *c, char *name, uint8_t size);

void col_float_init(struct col *c, char *name, uint8_t size);

void col_str_init(struct col *c, char *name, uint8_t size);

void col_blob_init(struct col *c, char *name);

#endif
