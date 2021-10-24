#ifndef _FUNCTION_DEF_H
#define _FUNCTION_DEF_H

#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include "../memory/variable.hpp"
#include "../types/type.hpp"
#include "../attributes.hpp"
#include "codeblock.hpp"

llvm::AllocaInst* create_entry_block_alloca(llvm::Function *in_func, std::string name, llvm::Type* t);

namespace AST {
	class Function {
		private:
		std::string name;
		std::vector<std::shared_ptr<AST::Variable>> args;
		AST::TypeInstance returnType;
		attributes_t attributes;

		std::unique_ptr<AST::CodeBlock> body;

		public:
		llvm::Function *generated;

		Function(std::string name, std::vector<std::shared_ptr<AST::Variable>> args, AST::TypeInstance returnType, attributes_t attributes, std::unique_ptr<AST::CodeBlock> body): name(name), args(args), returnType(returnType), attributes(attributes), body(std::move(body)), generated(nullptr) {}
		void codegen_prototype();
		virtual llvm::Value* codegen();
		AST::TypeInstance getRetType() { return returnType; } ;
		virtual std::string get_name() { return name; }
	};
}

namespace Parser {
	std::shared_ptr<AST::Function> parse_function(FILE* f, std::optional<AST::TypeInstance> self_type = std::optional<AST::TypeInstance>());
	//std::unique_ptr<AST::FunctionPrototype> parse_function_prototype(bool is_inline = false);
}

#endif