#ifndef INC_CMD_H
#define INC_CMD_H

#include "table.h"

struct db {
	char name[32];
	uint16_t size;
	struct table *tables;
	struct db_ix* ix;
};

struct db_ix {
	int (*open)(struct db*, char*);
	int (*table_open)(struct db*, struct table*, char*);
	int (*table_save)(struct db*, struct table*, char*);
};

int db_open(struct db *d, struct db_ix *ix, char *path);

void db_table_init(struct db *d, char *tname, uint16_t size);

struct table *db_table_get(struct db *d, char *tname);

void db_table_free(struct db *d, char *tname);

#endif

