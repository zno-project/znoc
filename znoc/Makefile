CFLAGS ?=
LDFLAGS ?=
CF = -MMD -MP --std=c++17 `llvm-config --cppflags | sed 's/-I/-isystem /g'` -g
LDF = -g -lfmt `llvm-config --ldflags --libs core native`

CXX?=clang++

SRCS = $(wildcard *.cpp)
SRCS_CONSTRUCT = $(wildcard constructions/*.cpp)
SRCS_TYPES = $(wildcard types/*.cpp)

OBJS = $(patsubst %.cpp, build/%.o, $(SRCS))
OBJS_CONSTRUCT = $(patsubst constructions/%.cpp, build/constructions/%.o, $(SRCS_CONSTRUCT))
OBJS_TYPES = $(patsubst types/%.cpp, build/types/%.o, $(SRCS_TYPES))

DEPENDS = $(patsubst %.cpp, build/%.d, $(SRCS))
DEPENDS_CONSTRUCT = $(patsubst constructions/%.cpp, build/constructions/%.d, $(SRCS_CONSTRUCT))
DEPENDS_TYPES = $(patsubst types/%.cpp, build/types/%.d, $(SRCS_TYPES))

.PHONY: all clean

all: znoc

sources:
	@echo $(SRCS) $(SRCS_CONSTRUCT) $(SRCS_TYPES)

clean:
	rm -rf build

-include ($(DEPENDS) $(DEPENDS_CONSTRUCT) $(DEPENDS_TYPES))

build/%.o: %.cpp
	mkdir -p build/
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/constructions/%.o: constructions/%.cpp
	mkdir -p build/constructions
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

build/types/%.o: types/%.cpp
	mkdir -p build/types
	$(CXX) $< -o $@ $(CFLAGS) $(CF) -c

znoc: $(OBJS) $(OBJS_CONSTRUCT) $(OBJS_TYPES)
	$(CXX) -o znoc $(OBJS) $(OBJS_CONSTRUCT) $(OBJS_TYPES) $(LDFLAGS) $(LDF)
