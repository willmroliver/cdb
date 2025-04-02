#include "db_fs.h"
#include "col.h"
#include "table.h"
#include "utils/fs.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

// --- HELPERS ---

void gen_table_file();

// --- UNIT TESTS ---

int db_open_test();

int main() {
  int passed =
    db_open_test();

  printf(passed ? "db_fs tests passed\n" : "db_fs tests failed\n");
  return 0;
}

int db_open_test() {
  int passed;
  struct db d;
  struct db_fs fs;
  struct db_ix *ix = (struct db_ix*)&fs;
  db_fs_init(ix);

  gen_table_file();

  db_open(&d, ix, "./dbs/db1");

  passed = 
    strcmp("db1", d.name) == 0 &&
    d.size == 1 &&
    d.tables[0].size == 4 &&
    strcmp("table1", d.tables[0].name) == 0 &&
    strcmp("col0", d.tables[0].cols[0].name) == 0 &&
    strcmp("col1", d.tables[0].cols[1].name) == 0 &&
    strcmp("col2", d.tables[0].cols[2].name) == 0 &&
    strcmp("col3", d.tables[0].cols[3].name) == 0; 

  if (!passed) {
    printf("db_open_test failed\n");
  }

  return passed;
}

void gen_table_file() {
  char buf[256], *path = "./dbs/db1", *tname = "table1";
  struct col_schema cs;
  struct table_schema ts;
  struct table t;
  FILE *f;

  assert(mkdir_recursive(path) == 0);
  snprintf(buf, sizeof(buf), "%s/%s", path, tname);
  f = fopen(buf, "w");

  table_init(&t, tname, 4);

  for (uint16_t i = 0; i < t.size; ++i) {
    col_init(t.cols + i, COL_INT, "", 32);
    snprintf(t.cols[i].name, sizeof(t.cols[i].name), "col%u", i);
  }

  table_serialize(&t, &ts);

  assert(fwrite(ts.data, TABLE_SCHEMA_SIZE, 1, f) > 0);
  assert(fputc('\n', f) > 0);
  
  for (uint16_t i = 0; i < t.size; ++i) {
    col_serialize(t.cols + i, &cs);

    assert(fwrite(cs.data, COL_SCHEMA_SIZE, 1, f) > 0);
    assert(fputc('\n', f) > 0);
  }

  fclose(f);
}

