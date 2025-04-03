#include "col.h"

#include <stdio.h>
#include <string.h>

// --- INTEGRATION TESTS ---
int col_serialize_parse_test(void);
int col_read_write_test(void);

int main(void)
{
  int passed = 
    col_serialize_parse_test() &&
    col_read_write_test();

  printf(passed ? "col tests passed\n" : "col tests failed\n");
  
  return 0;
}

// --- INTEGRATION TESTS ---

int col_serialize_parse_test(void)
{
  struct col_schema s;
  struct col t, u;
  char *cname = "test_col";

  col_init(&t, COL_INT, cname, 0);
  col_serialize(&t, &s);
  col_parse(&u, &s);

  return 
    strcmp(cname, t.name) == 0 && 
    strcmp(t.name, u.name) == 0 && 
    t.size == u.size;
}

int col_read_write_test(void)
{
  struct col_schema r, s;
  struct col t, u;
  char data[COL_SCHEMA_SIZE];
  char *cname = "test_col";

  col_init(&t, COL_INT, cname, 0);
  col_serialize(&t, &r);
  col_schema_write(&r, data);

  col_schema_read(&s, data);
  col_parse(&u, &s);

  return 
    strcmp(cname, t.name) == 0 &&
    strcmp(t.name, u.name) == 0 &&
    t.size == u.size;
}

