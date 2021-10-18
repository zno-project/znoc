CFLAGS ?=
LDFLAGS ?=
ifeq ($(shell uname),Darwin)
CF = -MMD -MP --std=c++20 `llvm-config --cppflags | sed 's/-I/-isystem /g'` -g
else
CF = -MMD -MP --std=c++20 `llvm-config --cppflags`
endif
LDF = -g -lfmt `llvm-config --ldflags --libs core native`

CXX?=clang++

SRCS = $(wildcard *.cpp)
SRCS_CONSTRUCT = $(wildcard constructions/*.cpp)
SRCS_TYPES = $(wildcard types/*.cpp)
SRCS_MEMORY = $(wildcard memory/*.cpp)

OBJS = $(patsubst %.cpp, build/%.o, $(SRCS))
OBJS_CONSTRUCT = $(patsubst constructions/%.cpp, build/constructions/%.o, $(SRCS_CONSTRUCT))
OBJS_TYPES = $(patsubst types/%.cpp, build/types/%.o, $(SRCS_TYPES))
OBJS_MEMORY = $(patsubst memory/%.cpp, build/memory/%.o, $(SRCS_MEMORY))

DEPENDS = $(patsubst %.cpp, build/%.d, $(SRCS))
DEPENDS_CONSTRUCT = $(patsubst constructions/%.cpp, build/constructions/%.d, $(SRCS_CONSTRUCT))
DEPENDS_TYPES = $(patsubst types/%.cpp, build/types/%.d, $(SRCS_TYPES))
DEPENDS_MEMORY = $(patsubst memory/%.cpp, buils/memory/%.d, $(SRCS_MEMORY))

TESTS = $(filter-out test/stdio.zno, $(wildcard test/*.zno))
TESTS_LOG = test/log.log

.PHONY: all clean test $(TESTS)

all: znoc

test: $(TESTS)

$(TESTS): znoc
	./znoc $@
	clang trampoline.c output.o -o build/test
	echo "" >> $(TESTS_LOG)
	echo "====== LOG FOR:" $@ "======" >> $(TESTS_LOG)
	-./build/test >> $(TESTS_LOG)

clean:
	rm -rf build
	rm znoc

-include ($(DEPENDS) $(DEPENDS_CONSTRUCT) $(DEPENDS_TYPES) $(DEPENDS_MEMORY))

build/%.o: %.cpp
	mkdir -p build/
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/constructions/%.o: constructions/%.cpp
	mkdir -p build/constructions
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/types/%.o: types/%.cpp
	mkdir -p build/types
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/memory/%.o: memory/%.cpp
	mkdir -p build/memory
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

znoc: $(OBJS) $(OBJS_CONSTRUCT) $(OBJS_TYPES) $(OBJS_MEMORY)
	$(CXX) -o znoc $(OBJS) $(OBJS_CONSTRUCT) $(OBJS_TYPES) $(OBJS_MEMORY) $(LDFLAGS) $(LDF)
