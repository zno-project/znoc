#ifndef _ZNO_TEST_FRAMEWORK_H
#define _ZNO_TEST_FRAMEWORK_H

#define private public
#define protected public

#include <main.hpp>
#include <memory/memory_location.hpp>
#include <types/type.hpp>
#include <types/type_base.hpp>
#include <constructions/namespace.hpp>
#include <parsing.hpp>
#include <location.hpp>
#include <constructions/expression.hpp>
#include "win_posix.hpp"

#include <deque>
#include <map>
#include <string>
#include <memory>

#include <string.h>

extern std::map<std::string, char> lastChars;

class CompilerGlobals {
 public:
  CompilerGlobals() {
		push_new_scope();
		LexLoc = {1, 0};
		lastChars = std::map<std::string, char>();
		GlobalNamespace = std::make_shared<AST::Namespace>("_G");
		AST::init_builtin_types();
  }

  virtual ~CompilerGlobals() {
	  stack_allocations.clear();
	  GlobalNamespace = nullptr;
	  mergeBB = nullptr;
	  condBB = nullptr;
  }

  // void TearDown() override {}
};

class CompilerMain: CompilerGlobals {
 public:
	zno_ifile f;

  CompilerMain(char *test_data): f(fmemopen((void*)test_data, strlen(test_data), "r"), "test") {
		get_next_token(f);
  }

  CompilerMain(const char *test_data): f(fmemopen((void*)test_data, strlen(test_data), "r"), "test") {
		get_next_token(f);
  }

  virtual ~CompilerMain() {
	  fclose(f.f);
  };

  // void TearDown() override {}
};

#endif
