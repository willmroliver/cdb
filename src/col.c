#include "col.h"

#include <string.h>
#include <stdlib.h>

struct col_schema *col_schema_alloc(void)
{
	return (struct col_schema*)calloc(1, COL_SCHEMA_SIZE);
}

void col_schema_free(struct col_schema *s)
{
	if (s != NULL)
		free(s);

	return;
}

void col_schema_read(struct col_schema *s, const void *src)
{
	memcpy(s, src, COL_SCHEMA_SIZE);
}

void col_schema_write(const struct col_schema *s, void *dest)
{
	memcpy(dest, s, COL_SCHEMA_SIZE);
}

int col_schema_fread(struct col_schema *s, FILE *f)
{
	if (fread(s->data, COL_SCHEMA_SIZE, 1, f) != 1)
		return -1;
	if (fgetc(f) != '\n')
		return -1;

	return 0;
}

int col_schema_fwrite(const struct col_schema *s, FILE *f)
{
	if (fwrite(s->data, COL_SCHEMA_SIZE, 1, f) != 1)
		return -1;
	if (fputc('\n', f) != '\n')
		return -1;

	return 0;
}

uint8_t col_serialize(const struct col *c, struct col_schema *s)
{
	uint8_t at;

	if (c == NULL || s == NULL)
		return 0;

	at = 0;

	memcpy(s->data + at, &c->name, COL_NAME_BYTES);
	at += COL_NAME_BYTES;
	memcpy(s->data + at, &c->type, COL_TYPE_BYTES);
	at += COL_TYPE_BYTES;
	memcpy(s->data + at, &c->size, COL_SIZE_BYTES);
	at += COL_SIZE_BYTES;

	return at;
}

uint8_t col_parse(struct col *c, const struct col_schema *s)
{
	uint8_t at;

	if (c == NULL || s == NULL)
		return 0;

	at = 0;

	memcpy(&c->name, s->data + at, COL_NAME_BYTES);
	at += COL_NAME_BYTES;
	memcpy(&c->type, s->data + at, COL_TYPE_BYTES);
	at += COL_TYPE_BYTES;
	memcpy(&c->size, s->data + at, COL_SIZE_BYTES);
	at += COL_SIZE_BYTES;

	return at;
}

void col_init(struct col *c, col_t t, char *name, uint8_t size)
{
	size_t min;

	if (c == NULL)
		return;

	c->type = (uint8_t)t;
	c->size = size;

	min = strlen(name);
	if (min > COL_NAME_BYTES - 1)
		min = COL_NAME_BYTES - 1;

	memcpy(c->name, name, min);
	c->name[min] = '\0';
}

void col_val_free(struct col *c)
{
	if (c != NULL)
		if (c->val != NULL)
			free(c->val);
}

void col_int_init(struct col *c, char *name, uint8_t size)
{
	col_init(c, COL_INT, name, size);
}

void col_float_init(struct col *c, char *name, uint8_t size)
{
	col_init(c, COL_FLOAT, name, size);
}

void col_str_init(struct col *c, char *name, uint8_t size)
{
	col_init(c, COL_STR, name, size);
}

void col_blob_init(struct col *c, char *name)
{
	col_init(c, COL_BLOB, name, 0);
}

