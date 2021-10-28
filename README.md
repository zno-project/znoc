# ZnO

ZnO is a language that aims to combine speed and safety.

It makes use of libLLVM.

This repo contains the compiler, `znoc`.

To build:
- Clone https://github.com/egkoppel/ZnO/
- `make`

This will create the `znoc` executable in `build/`.
It can then be used as follows:

`path/to/znoc infile.zno outfile.o`

After this it is necessary to link with some trampoline code (`trampoline.c`) and the C standard library. This can be achieved with:

`clang trampoline.c outfile.o -o my_program`

There are also unit and regression tests, which can be run with `make test` and `make check` respectively.
