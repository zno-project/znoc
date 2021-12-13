#ifndef _FUNCTION_DEF_H
#define _FUNCTION_DEF_H

#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include "../memory/variable.hpp"
#include "../memory/memory_location.hpp"
#include "../types/type.hpp"
#include "../attributes.hpp"
#include "codeblock.hpp"

llvm::AllocaInst* create_entry_block_alloca(llvm::Function *in_func, std::string name, llvm::Type* t);

namespace AST {
	class Function: public MemoryLoc {
		private:
		std::vector<std::shared_ptr<AST::Variable>> args;
		AST::TypeInstance returnType;
		std::unique_ptr<AST::CodeBlock> body;
		bool is_member_func;
		bool is_variadic;
		attributes_t attributes;

		public:
		Function(std::string name, std::vector<std::shared_ptr<AST::Variable>> args, AST::TypeInstance returnType, attributes_t attributes, std::unique_ptr<AST::CodeBlock> body, bool is_member_func, bool is_variadic): MemoryLoc(returnType.get_function_returning(), name), args(args), returnType(returnType), body(std::move(body)), is_member_func(is_member_func), is_variadic(is_variadic), attributes(attributes) {}
		void codegen_prototype();
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
		AST::TypeInstance getRetType() { return returnType; } ;
		virtual std::string get_name() { return name; }
	};
}

namespace Parser {
	std::shared_ptr<AST::Function> parse_function(FILE* f, std::optional<AST::TypeInstance> self_type = std::optional<AST::TypeInstance>());
}

#endif
