#include <stdio.h>
#include <unwind.h>
#include <stdlib.h>

void printI(uint32_t x) {
	printf("%i\n", x);
}

uint32_t* allocate() {
	return malloc(sizeof(uint32_t));
}

void deallocate(uint32_t *p) {
	free(p);
}

void set_ptr(uint32_t *p, uint32_t v) {
	*p = v;
}