#ifndef INC_TABLES_H
#define INC_TABLES_H

#include "col.h"

#include <stdio.h>

#define TABLE_SCHEMA_SIZE 64
#define TABLE_NAME_BYTES 32
#define TABLE_SIZE_BYTES 2

struct table {
  char name[32];
  uint16_t size;
  struct col *cols;
};

struct table_schema {
  char data[TABLE_SCHEMA_SIZE];  
};

struct table_schema *table_schema_alloc(void);

void table_schema_free(struct table_schema *s);

void table_schema_read(struct table_schema *s, const void *src);

void table_schema_write(const struct table_schema *s, void *dest);

int table_schema_fread(struct table_schema *s, FILE *f);

int table_schema_fwrite(const struct table_schema *s, FILE *f);

uint8_t table_serialize(const struct table *t, struct table_schema *buf);

uint8_t table_parse(struct table *t, const struct table_schema *buf);

void table_init(struct table *t, char *name, uint16_t size);

void table_free(struct table *t);

#endif
