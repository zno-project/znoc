#ifndef _MEMORY_LOCATION_H
#define _MEMORY_LOCATION_H

#include "../constructions/expression.hpp"
#include "../types/type.hpp"
#include "../types/type.hpp"

#include <map>
#include <deque>
#include <string>
#include <memory>

#include <llvm/IR/Value.h>

namespace AST {
	class MemoryLoc {
		protected:
		public:
		std::string name;
		llvm::Value *allocaV;
		AST::TypeInstance underlying_type;
		
		MemoryLoc(AST::TypeInstance underlying_type, std::string name): name(name), allocaV(nullptr), underlying_type(underlying_type) {};
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder) = 0;
		virtual ~MemoryLoc() = default;
	};
}

extern std::deque<std::map<std::string, std::shared_ptr<AST::MemoryLoc>>> stack_allocations;
void push_new_scope();

namespace AST { class Variable; }

class ScopeEnd: public AST::Expression {
	private:
	std::vector<std::shared_ptr<AST::Variable>> out_of_scope_vars;

	public:
	ScopeEnd(std::vector<std::shared_ptr<AST::Variable>> out_of_scope_vars) : Expression(AST::get_fundamental_type("void")), out_of_scope_vars(std::move(out_of_scope_vars)) {}
	llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
};

std::unique_ptr<ScopeEnd> pop_scope();

#endif
