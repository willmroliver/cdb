#ifndef INC_DS_DL_LIST_H
#define INC_DS_DL_LIST_H

#include <stddef.h>

typedef struct dl_node {
	void *val;
	struct dl_node *prev;
	struct dl_node *next;
} dl_node_t;

typedef struct dl_list dl_list_t;

dl_list_t *dl_list_alloc();

void dl_list_free(dl_list_t *l);

dl_node_t *dl_list_insert(dl_list_t *l, void *val);

void dl_list_remove(dl_list_t *l, dl_node_t *n);

dl_node_t *dl_list_find(dl_list_t *l, void *val);

int dl_list_empty(dl_list_t *l);

dl_node_t *dl_list_next(dl_list_t *l, dl_node_t* n);

dl_node_t *dl_list_prev(dl_list_t *l, dl_node_t* n);

#endif

