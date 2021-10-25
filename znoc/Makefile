CFLAGS ?=
LDFLAGS ?=
ifeq ($(shell uname),Darwin)
CF = -MMD -MP --std=gnu++17 `llvm-config --cppflags | sed 's/-I/-isystem /g'` -g
else
CF = -MMD -MP --std=gnu++17 `llvm-config --cppflags`
endif
LDF = -g -lfmt `llvm-config --ldflags --libs core native`

CXX?=clang++

SRCS = $(wildcard src/*.cpp)
SRCS_CONSTRUCT = $(wildcard src/constructions/*.cpp)
SRCS_TYPES = $(wildcard src/types/*.cpp)
SRCS_MEMORY = $(wildcard src/memory/*.cpp)

OBJS = $(patsubst src/%.cpp, build/%.o, $(SRCS))
OBJS_CONSTRUCT = $(patsubst src/constructions/%.cpp, build/constructions/%.o, $(SRCS_CONSTRUCT))
OBJS_TYPES = $(patsubst src/types/%.cpp, build/types/%.o, $(SRCS_TYPES))
OBJS_MEMORY = $(patsubst src/memory/%.cpp, build/memory/%.o, $(SRCS_MEMORY))

DEPENDS = $(patsubst src/%.cpp, build/%.d, $(SRCS))
DEPENDS_CONSTRUCT = $(patsubst src/constructions/%.cpp, build/constructions/%.d, $(SRCS_CONSTRUCT))
DEPENDS_TYPES = $(patsubst src/types/%.cpp, build/types/%.d, $(SRCS_TYPES))
DEPENDS_MEMORY = $(patsubst src/memory/%.cpp, buils/memory/%.d, $(SRCS_MEMORY))

TESTS = $(filter-out test/stdio.zno, $(wildcard test/*.zno))
TESTS_LOG = test/log.log

.PHONY: all clean

all: build/znoc

clean:
	rm -rf build

-include ($(DEPENDS) $(DEPENDS_CONSTRUCT) $(DEPENDS_TYPES) $(DEPENDS_MEMORY))

build:
	mkdir -p build/
build/constructions: build
	mkdir -p build/constructions/
build/memory: build
	mkdir -p build/memory
build/types: build
	mkdir -p build/types

build/%.o: src/%.cpp | build
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/constructions/%.o: src/constructions/%.cpp | build/constructions
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/types/%.o: src/types/%.cpp | build/types
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/memory/%.o: src/memory/%.cpp | build/memory
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/znoc: $(OBJS) $(OBJS_CONSTRUCT) $(OBJS_TYPES) $(OBJS_MEMORY) | build
	$(CXX) -o build/znoc $(OBJS) $(OBJS_CONSTRUCT) $(OBJS_TYPES) $(OBJS_MEMORY) $(LDFLAGS) $(LDF)
