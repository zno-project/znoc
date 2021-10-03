#include <stdio.h>
struct Exception {};

#ifdef __cplusplus
extern "C" {
#endif

    void seppuku();

#ifdef __cplusplus
}
#endif

struct Fake_Exception {};

void raise() {
    throw Exception();
}

void try_but_dont_catch() {
	
        raise();
}

void catchit() {
    try {
        try_but_dont_catch();
    } catch(Exception&) {
        printf("Caught an Exception!\n");
    }

    printf("catchit handled the exception\n");
}

extern "C" {
    void seppuku() {
        catchit();
    }
}

int main()
{
    seppuku();
    return 0;
}