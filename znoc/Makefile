CXX ?= clang++
PKG_CONFIG ?= pkg-config
LLVM_CONFIG ?= llvm-config

ifeq ($(shell uname),Darwin)
CF = -MMD -MP --std=gnu++17 `$(LLVM_CONFIG) --cppflags | sed 's/-I/-isystem /g'` `$(PKG_CONFIG) --cflags fmt` -g
else
CF = -MMD -MP --std=gnu++17 `$(LLVM_CONFIG) --cppflags` `$(PKG_CONFIG) --cflags fmt` -g
endif
LDF = -g `$(LLVM_CONFIG) --ldflags --libs core native` `$(PKG_CONFIG) --libs fmt`

CFLAGS ?=
LDFLAGS ?=

SRCS = $(wildcard src/*.cpp)
SRCS_CONSTRUCT = $(wildcard src/constructions/*.cpp)
SRCS_TYPES = $(wildcard src/types/*.cpp)
SRCS_MEMORY = $(wildcard src/memory/*.cpp)

OBJS = $(patsubst src/%.cpp, build/%.o, $(SRCS))
OBJS_CONSTRUCT = $(patsubst src/constructions/%.cpp, build/constructions/%.o, $(SRCS_CONSTRUCT))
OBJS_TYPES = $(patsubst src/types/%.cpp, build/types/%.o, $(SRCS_TYPES))
OBJS_MEMORY = $(patsubst src/memory/%.cpp, build/memory/%.o, $(SRCS_MEMORY))
OBJS_TEST = $(filter-out build/main.o, $(OBJS)) $(OBJS_CONSTRUCT) $(OBJS_TYPES) $(OBJS_MEMORY)

DEPENDS = $(patsubst src/%.cpp, build/%.d, $(SRCS))
DEPENDS_CONSTRUCT = $(patsubst src/constructions/%.cpp, build/constructions/%.d, $(SRCS_CONSTRUCT))
DEPENDS_TYPES = $(patsubst src/types/%.cpp, build/types/%.d, $(SRCS_TYPES))
DEPENDS_MEMORY = $(patsubst src/memory/%.cpp, build/memory/%.d, $(SRCS_MEMORY))

GTEST = test/unit/gtest
GTEST_LIB = test/unit/build/lib
UNIT_TESTS = $(patsubst test/unit/%.cpp, test/unit/build/znoc-test-%, $(wildcard test/unit/*.cpp))

REGRESSION_TESTS = $(filter-out test/regression/stdio.zno, $(wildcard test/regression/*.zno))
REGRESSION_TESTS_LOG = test/regression/log.log

.PHONY: all clean test

all: build/znoc

clean:
	rm -rf build test/unit/build

test: $(UNIT_TESTS)

-include ($(DEPENDS) $(DEPENDS_CONSTRUCT) $(DEPENDS_TYPES) $(DEPENDS_MEMORY))

## directories

build:
	mkdir -p build/
build/constructions:
	mkdir -p build/constructions/
build/memory:
	mkdir -p build/memory
build/types:
	mkdir -p build/types

test/unit/build:
	mkdir -p test/unit/build

## znoc

build/%.o: src/%.cpp | build
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/constructions/%.o: src/constructions/%.cpp | build/constructions
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/types/%.o: src/types/%.cpp | build/types
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/memory/%.o: src/memory/%.cpp | build/memory
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/znoc: $(OBJS) $(OBJS_CONSTRUCT) $(OBJS_TYPES) $(OBJS_MEMORY) | build
	$(CXX) -o $@ $(OBJS) $(OBJS_CONSTRUCT) $(OBJS_TYPES) $(OBJS_MEMORY) $(LDFLAGS) $(LDF)

## tests

$(GTEST_LIB)/libgtest.a $(GTEST_LIB)/libgtest_main.a: $(GTEST)/googletest/src/* | test/unit/build
	cd test/unit/build && cmake ../gtest
	$(MAKE) -C test/unit/build gtest gtest_main
	touch $(GTEST_LIB)/libgtest.a
$(GTEST_LIB)/libgtest.a: $(GTEST_LIB)/libgtest_main.a    ## prevent make trying to build these in parallel

.SECONDARY: $(patsubst test/unit/%.cpp, test/unit/build/%.o, $(wildcard test/unit/*.cpp))    ## prevent make from automatically removing "intermediate" object files
test/unit/build/%.o: test/unit/%.cpp | test/unit/build
	$(CXX) -I$(GTEST)/googletest/include -Isrc $< -o $@ $(CFLAGS) $(CF) -c

test/unit/build/znoc-test-%: test/unit/build/%.o $(OBJS_TEST) $(GTEST_LIB)/libgtest.a $(GTEST_LIB)/libgtest_main.a | test/unit/build
	$(CXX) -o $@ $< $(OBJS_TEST) -L$(GTEST_LIB) -lgtest -lgtest_main $(LDFLAGS) $(LDF) -lpthread
