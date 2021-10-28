#include "main.hpp"
#include "memory/memory_location.hpp"
#include "types/type.hpp"
#include "constructions/namespace.hpp"
#include "parsing.hpp"
#include "location.hpp"
#include "constructions/expression.hpp"
#include "win_posix.hpp"

#include <deque>
#include <map>
#include <string>
#include <memory>

#include <string.h>

std::unique_ptr<AST::Namespace> GlobalNamespace;
extern std::map<FILE*, char> lastChars;

class CompilerGlobals {
 public:
  CompilerGlobals() {
		LexLoc = {1, 0};
		lastChars = std::map<FILE*, char>();
		stack_allocations.push_front(std::map<std::string, std::shared_ptr<AST::MemoryLoc>>());
		GlobalNamespace = std::make_unique<AST::Namespace>("_G");
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
	FILE *f;

  CompilerMain(char *test_data) {
		f = fmemopen((void*)test_data, strlen(test_data), "r");
		get_next_token(f);
  }

  CompilerMain(const char *test_data) {
		f = fmemopen((void*)test_data, strlen(test_data), "r");
		get_next_token(f);
  }

  virtual ~CompilerMain() {
	  fclose(f);
  };

  // void TearDown() override {}
};
