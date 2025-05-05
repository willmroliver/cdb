#ifndef INC_DS_DLIST_H
#define INC_DS_DLIST_H

#include <stddef.h>

typedef struct dnode {
	void *val;
	struct dnode *prev;
	struct dnode *next;
} dnode_t;

typedef struct dlist dlist_t;

dlist_t *dlist_alloc();

void dlist_free(dlist_t *l);

dnode_t *dlist_insert(dlist_t *l, void *val);

void dlist_remove(dnode_t *n);

dnode_t *dlist_find(dlist_t *l, void *val);

#endif

