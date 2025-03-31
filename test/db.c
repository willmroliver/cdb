#include "db.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// --- INTEGRATION TESTS ---
int db_table_init_get_free_test();

int main() {
  int passed =
    db_table_init_get_free_test();

  printf(passed ? "db tests passed\n" : "db tests failed\n");

  return 0;
}

// --- INTEGRATION TESTS ---

int db_table_init_get_free_test() {
  int ntables = 4, i, passed;
  struct db d = {
    .name = "test_db",
    .size = 0,
  };
  char cbuf[16] = { '\0' }, *prefix = "table_";

  // initialize some tables with enumerated names
  for (i = 0; i < ntables; ++i) {
    snprintf(cbuf, sizeof(cbuf), "%s%d", prefix, i);
    db_table_init(&d, cbuf, 0);
  }

  passed = d.size == ntables;

  // check tables initialized
  for (i = 0; i < d.size; ++i) {
    snprintf(cbuf, sizeof(cbuf), "%s%d", prefix, i);

    passed = passed &&
      strcmp(d.tables[i].name, cbuf) == 0;
  }

  // pick a table at random and search for it
  srand(time(NULL));
  i = rand() % ntables;
  snprintf(cbuf, sizeof(cbuf), "%s%d", prefix, i); 

  struct table *t = db_table_get(&d, cbuf);
  passed = passed &&
    t == d.tables + i;

  // free the found table and search again
  db_table_free(&d, cbuf);
  t = db_table_get(&d, cbuf);
  passed = passed &&
    d.size == ntables - 1 &&
    t == NULL;

  return passed;
}
