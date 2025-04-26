#include "ds/ring.h"

#include <stdio.h>

int main() 
{
	unsigned int at = 0, i = 0, val = 0, vals[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	ring_t *buf = ring_alloc(7, sizeof(vals[0]));

	if (buf != NULL) {
		printf("buf alloc should have failed but did not\n");
		return 1;
	}

	buf = ring_alloc(8, sizeof(vals[0]));
	if (buf == NULL) {
		printf("buf alloc should have succeeded but did not\n");
		return 1;
	}

	for (; at < 8; ++at) {
		if (ring_push(buf, vals + at)) {
			printf("failed to push el %u\n", at);
			return 1;
		}
	}

	if (ring_push(buf, vals + at) == 0) {
		printf("ring push should have failed but did not\n");
		return 1;
	}

	for (; at > 0; --at) {
		if (ring_pop(buf, &val)) {
			printf("failed to pop el %u\n", at-1);
			return 1;
		}

		if (val != 8 - at) {
			printf("expected %u, got %u\n", 8 - at, val);
			return 1;
		}
	}

	if (ring_pop(buf, &val) == 0) {
		printf("ring pop should have failed but did not\n");
		return 1;
	}

	for (; i < 16; ++i) {
		for (; at < 7; ++at) {
			if (ring_push(buf, vals + at)) {
				printf("failed to push el %u in iter %u\n", at, i);
				return 1;
			}
		}

		for (; at > 0; --at) {
			if (ring_pop(buf, &val)) {
				printf("failed to pop el %u in iter %u\n", 7 - at, i);
				return 1;
			}

			if (val != 7 - at) {
				printf("expected %u, got %u in iter %u\n", 7 - at, val, i);
				return 1;
			}
		}
	}

	printf("ring tests passed\n");
	return 0;
}
