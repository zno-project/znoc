#include <stdarg.h>

void myThing(int num, ...) {
	va_list args;
	va_start(args, num);
	int first_arg = va_arg(args, int);
	printf("first arg is: %i\n", first_arg);
	va_list args2;
	va_copy(args2, args);
	int second_arg = va_arg(args, int);
	int second_argc = va_arg(args2, int);
	printf("second arg is: %i\n", second_arg);
	printf("second arg copy is: %i\n", second_argc);
}

int main() {
	myThing(5, 1, 2, 3, 4, 5);
}