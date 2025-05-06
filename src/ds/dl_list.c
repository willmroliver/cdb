#include "ds/dl_list.h"

#include <stdlib.h>
#include <string.h>

struct dl_list {
	struct dl_node *head;
	struct dl_node *tail;
};

dl_list_t *dl_list_alloc()
{
	dl_list_t *l = malloc(sizeof(dl_list_t));
	l->head = malloc(sizeof(dl_node_t) * 2);
	l->tail = l->head + 1;

	l->head->prev = NULL;
	l->head->next = l->tail;
	l->tail->next = NULL;
	l->tail->prev = l->head;

	return l;
}

void dl_list_free(dl_list_t *l)
{
	while (l->head->next != l->tail)
		dl_list_remove(l, l->head->next);
	free(l->head);
	free(l);
}

dl_node_t *dl_list_insert(dl_list_t *l, void *val)
{
	struct dl_node *n = malloc(sizeof(struct dl_node)), *prev = l->tail->prev; 
	n->val = val;

	n->prev = prev;
	prev->next = n;

	n->next = l->tail;
	l->tail->prev = n;

	return n;
}

void dl_list_remove(dl_list_t *l, dl_node_t *n)
{
	/* unused but signals intent and expected ownership */
	(void)l;

	n->prev->next = n->next;
	n->next->prev = n->prev;
	free(n);
}

dl_node_t *dl_list_find(dl_list_t *l, void *val)
{
	struct dl_node *n = l->head, *end = l->tail;
	while (n != end && n->val != val)
		n = n->next;
	return n == end ? NULL : n;
}

int dl_list_empty(dl_list_t *l)
{
	return l->head->next == l->tail;
}

dl_node_t *dl_list_next(dl_list_t *l, dl_node_t *n)
{
	if (n == NULL)
		n = l->head;
	return n->next == l->tail ? NULL : n->next;
}

dl_node_t *dl_list_prev(dl_list_t *l, dl_node_t *n)
{
	if (n == NULL)
		n = l->tail;
	return n->prev == l->head ? NULL : n->prev;
}

