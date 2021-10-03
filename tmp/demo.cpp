#include <stdio.h>
#include <exception>
#include <stdexcept>
#include <memory>

class Thing {
	public:
	Thing() {}
	~Thing() {
		printf("cleanup on catch\n");
	}
	void do_stuff() {printf("do stuff\n");}
	void do_more_stuff() {printf("do more stuff\n");}
	void do_even_more_stuff() {printf("do even more stuff\n");}
};

void test() {
	throw std::runtime_error("HELP MEEEEEEE!");
}

void test_wrapper() {
	auto a_thing = std::make_shared<Thing>();
	test();
	a_thing->do_stuff();
	a_thing->do_more_stuff();
	a_thing->do_even_more_stuff();
}

int main() {
	try {
		printf("Throwing exception in main\n");
		test_wrapper();
	} catch (std::exception &e) {
		printf("Caught exception in main\n");
	}
}