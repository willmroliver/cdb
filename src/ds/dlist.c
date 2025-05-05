#include "ds/dlist.h"

#include <stdlib.h>
#include <string.h>

struct dlist {
	struct dnode *head;
	struct dnode *tail;
};

dlist_t *dlist_alloc()
{
	dlist_t *l = malloc(sizeof(dlist_t));
	l->head = malloc(sizeof(dnode_t) * 2);
	l->tail = l->head + 1;

	l->head->prev = NULL;
	l->head->next = l->tail;
	l->tail->next = NULL;
	l->tail->prev = l->head;

	return l;
}

void dlist_free(dlist_t *l)
{
	while (l->head->next != l->tail)
		dlist_remove(l->head->next);
	free(l->head);
	free(l);
}

dnode_t *dlist_insert(dlist_t *l, void *val)
{
	struct dnode *n = malloc(sizeof(struct dnode)), *prev = l->tail->prev; 
	n->val = val;

	n->prev = prev;
	prev->next = n;

	n->next = l->tail;
	l->tail->prev = n;

	return n;
}

void dlist_remove(dnode_t *n)
{
	n->prev->next = n->next;
	n->next->prev = n->prev;
	free(n);
}

dnode_t *dlist_find(dlist_t *l, void *val)
{
	struct dnode *n = l->head, *end = l->tail;
	while (n != end && n->val != val)
		n = n->next;
	return n == end ? NULL : n;
}
