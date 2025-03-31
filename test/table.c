#include "table.h"

#include <stdio.h>
#include <string.h>

// --- INTEGRATION TESTS ---
int table_serialize_parse_test();
int table_read_write_test();

int main() {
  int passed = 
    table_serialize_parse_test() &&
    table_read_write_test();

  printf(passed ? "table tests passed\n" : "table tests failed\n");
  
  return 0;
}

// --- INTEGRATION TESTS ---

int table_serialize_parse_test() {
  struct table_schema s;
  struct table t, u;
  char *tname = "test_table";

  table_init(&t, tname, 0);
  table_serialize(&t, &s);
  table_parse(&u, &s);

  return 
    strcmp(tname, t.name) == 0 && 
    strcmp(t.name, u.name) == 0 && 
    t.size == u.size;
}

int table_read_write_test() {
  struct table_schema r, s;
  struct table t, u;
  char data[TABLE_SCHEMA_SIZE];
  char *tname = "test_table";

  table_init(&t, tname, 0);
  table_serialize(&t, &r);
  table_schema_write(&r, data);

  table_schema_read(&s, data);
  table_parse(&u, &s);

  return 
    strcmp(tname, t.name) == 0 &&
    strcmp(t.name, u.name) == 0 &&
    t.size == u.size;
}

