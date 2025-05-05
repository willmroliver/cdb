#include "ds/dlist.h"
#include <stdio.h>

/* --- INTEGRATION TESTS --- */

int insert_find_remove_test();

int main()
{
	int passed = 
		insert_find_remove_test();

	printf("dlist tests %s\n", passed ? "passed" : "failed");
	return 0;
}

/* --- INTEGRATION TESTS --- */

int insert_find_remove_test()
{
	int i, n = 10, passed = 1;
	int vals[n];
	dnode_t *node[n];
	dlist_t *l = dlist_alloc();

	for (i = 0; i < n; ++i) {
		vals[i] = i;
		node[i] = dlist_insert(l, vals + i);
		if (dlist_find(l, vals + i) != node[i]) {
			passed = 0;
			goto done;
		}
	}

	for (i = 7; i < n; ++i) {
		dlist_remove(node[i]);
		if (dlist_find(l, vals + i) != NULL) {
			passed = 0;
			goto done;
		}
	}

	for (i = 6; i > 3; --i) {
		dlist_remove(node[i]);
		if (dlist_find(l, vals + i) != NULL) {
			passed = 0;
			goto done;
		}
	}

	for (i = 0; i <= 3; ++i) {
		dlist_remove(node[i]);
		if (dlist_find(l, vals + i) != NULL) {
			passed = 0;
			goto done;
		}
	}
done:
	dlist_free(l);
	return passed;
}

