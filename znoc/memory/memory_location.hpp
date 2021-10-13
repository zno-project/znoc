#ifndef _MEMORY_LOCATION_H
#define _MEMORY_LOCATION_H

#include "../constructions/expression.hpp"
#include "../types/type.hpp"

#include <map>
#include <deque>
#include <string>

#include <llvm/IR/Value.h>

namespace AST {
	class MemoryLoc {
		protected:
		llvm::Value *allocaV;
		public:
		AST::TypeInstance underlying_type;
		
		MemoryLoc(AST::TypeInstance underlying_type): allocaV(nullptr), underlying_type(underlying_type) {};
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder) = 0;
		virtual ~MemoryLoc() = default;
	};
}

extern std::deque<std::map<std::string, std::shared_ptr<AST::MemoryLoc>>> stack_allocations;

#endif