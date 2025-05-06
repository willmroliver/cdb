#include "ds/dl_list.h"
#include <stdio.h>

/* --- INTEGRATION TESTS --- */

int insert_find_remove_test();

int main()
{
	int passed = 
		insert_find_remove_test();

	printf("dl_list tests %s\n", passed ? "passed" : "failed");
	return 0;
}

/* --- INTEGRATION TESTS --- */

int insert_find_remove_test()
{
	int i, n = 10, passed = 1;
	int vals[n];
	dl_node_t *node[n];
	dl_list_t *l = dl_list_alloc();

	for (i = 0; i < n; ++i) {
		vals[i] = i;
		node[i] = dl_list_insert(l, vals + i);
		if (dl_list_find(l, vals + i) != node[i]) {
			passed = 0;
			goto done;
		}
	}

	if (dl_list_empty(l)) {
		passed = 0;
		goto done;
	}

	/* remove mid to tail, forward */
	for (i = 7; i < n; ++i) {
		dl_list_remove(l, node[i]);
		if (dl_list_find(l, vals + i) != NULL) {
			passed = 0;
			goto done;
		}
	}

	/* remove from tail, backward */
	for (i = 6; i > 3; --i) {
		dl_list_remove(l, node[i]);
		if (dl_list_find(l, vals + i) != NULL) {
			passed = 0;
			goto done;
		}
	}

	/* remove from head, forward */
	for (i = 0; i <= 3; ++i) {
		dl_list_remove(l, node[i]);
		if (dl_list_find(l, vals + i) != NULL) {
			passed = 0;
			goto done;
		}
	}

	if (!dl_list_empty(l))
		passed = 0;
done:
	dl_list_free(l);
	return passed;
}

