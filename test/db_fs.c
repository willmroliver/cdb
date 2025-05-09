#include "db_fs.h"
#include "col.h"
#include "table.h"
#include "utils/fs.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/*--- HELPERS ---*/

void gen_table_file(char*, char*, char*, uint16_t, col_t, uint8_t);

/*--- UNIT TESTS ---*/

int db_open_test(void);
int db_fs_table_open_test(void);

int main(void)
{
	int passed =
		db_open_test() &&
		db_fs_table_open_test();

	printf(passed ? "db_fs tests passed\n" : "db_fs tests failed\n");
	return 0;
}

/*--- UNIT TESTS ---*/

int db_open_test(void)
{
	int passed;
	struct db d;
	struct db_fs fs;
	char buf[256];
	struct table *t;
	struct col *c;
	char *path = "./dbs", *dbname = "db1", *tname = "table1";
	uint16_t i, ncols = 4;
	col_t ctype = COL_INT;
	uint8_t csize = 32;

	db_fs_init(&fs);
	gen_table_file(path, dbname, tname, ncols, ctype, csize);

	snprintf(buf, sizeof(buf), "%s/%s", path, dbname);
	db_open(&d, &fs.ix, buf);

	passed = 
		strcmp(dbname, d.name) == 0 &&
		d.size == 1 &&
		strcmp(tname, d.tables[0].name) == 0 &&
		d.tables[0].size == ncols;

	t = d.tables;

	for (i = 0; i < ncols; ++i) {
		if (!passed)
			break;

		snprintf(buf, sizeof(buf), "col%u", i);
		c = t->cols + i;

		passed = passed &&
			strcmp(buf, c->name) == 0 &&
			c->size == csize &&
			c->type == ctype;
	}

	if (!passed)
		printf("db_open_test failed\n");

	return passed;
}

int db_fs_table_open_test(void)
{
	int passed;
	struct db d = { 0 };
	struct db_fs fs;
	struct db_ix *ix = (struct db_ix*)&fs;
	char buf[256];
	char *path = "./dbs", *dbname = "db2", *tname = "table2";
	uint16_t tsize = 2, i;
	uint8_t csize = 16;
	col_t ctype = COL_STR;
	struct table t;
	struct col *c;

	gen_table_file(path, dbname, tname, tsize, ctype, csize);
	db_fs_init(&fs);

	snprintf(d.name, sizeof(d.name), "%s", dbname);
	snprintf(fs.path, sizeof(fs.path), "%s/%s", path, dbname);
	d.ix = ix;

	db_fs_table_open(&d, &t, tname);

	passed = 
		strcmp(t.name, tname) == 0 &&
		t.size == tsize;

	for (i = 0; i < t.size; ++i) {
		if (!passed)
			break;

		c = t.cols + i;
		snprintf(buf, sizeof(buf), "col%u", i);

		passed = passed &&
			c->size == csize &&
			c->type == ctype &&
			strcmp(c->name, buf) == 0;
	}

	return passed;
}

/*--- HELPERS ---*/

void gen_table_file(char *path, 
		    char *dbname, 
		    char *tname,
		    uint16_t ncols,
		    col_t ctype,
		    uint8_t csize)
{
	char buf[256];
	FILE *f;
	uint16_t i;
	struct table t;
	struct table_schema ts;
	struct col_schema cs;

	snprintf(buf, sizeof(buf), "%s/%s", path, dbname);
	assert(mkdir_recursive(buf) == 0);

	snprintf(buf, sizeof(buf), "%s/%s/%s", path, dbname, tname);
	f = fopen(buf, "wb");
	assert(f != NULL);

	table_init(&t, tname, ncols);

	for (i = 0; i < t.size; ++i) {
		col_init(t.cols + i, ctype, "", csize);
		snprintf(t.cols[i].name, sizeof(t.cols[i].name), "col%u", i);
	}

	table_serialize(&t, &ts);

	assert(fwrite(ts.data, TABLE_SCHEMA_SIZE, 1, f) > 0);
	assert(fputc('\n', f) > 0);

	for (i = 0; i < t.size; ++i) {
		col_serialize(t.cols + i, &cs);

		assert(fwrite(cs.data, COL_SCHEMA_SIZE, 1, f) > 0);
		assert(fputc('\n', f) > 0);
	}

	fclose(f);
}

