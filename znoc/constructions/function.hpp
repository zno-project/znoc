#ifndef _FUNCTION_H
#define _FUNCTION_H

#include "expression.hpp"
#include "../types/type.hpp"
#include "codeblock.hpp"
#include "variable.hpp"
#include "../attributes.hpp"
#include "../mangling.hpp"

#include <map>
#include <string>
#include <vector>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>

llvm::AllocaInst* create_entry_block_alloca(llvm::Function *in_func, std::string name, llvm::Type* t);

namespace AST {
	class Function {
		std::string name;
		std::vector<std::shared_ptr<AST::Variable>> args;
		AST::type_usage_t returnType;
		attributes_t attributes;

		std::unique_ptr<AST::CodeBlock> body;

		public:
		llvm::Function *generated;

		Function(std::string name, std::vector<std::shared_ptr<AST::Variable>> args, AST::type_usage_t returnType, attributes_t attributes, std::unique_ptr<AST::CodeBlock> body): name(name), args(args), returnType(returnType), attributes(attributes), body(std::move(body)), generated(nullptr) {}
		void codegen_prototype();
		virtual llvm::Value* codegen();
		AST::type_usage_t getRetType();
		//std::string mangled_name();
		virtual const std::string& get_name() { return name; }
	};
}

namespace AST {
	AST::Function* get_function_by_name(std::string name);

	class FunctionCall: public AST::Expression {
		std::shared_ptr<AST::Function> function;
		std::vector<std::unique_ptr<AST::Expression>> args;

		public:
		//FunctionCall(std::string callee, std::vector<std::unique_ptr<AST::Expression>> args): Expression(AST::get_type_by_name("__llvm_i32")), function(AST::get_function_by_name(callee)), args(std::move(args)) {}
		FunctionCall(std::shared_ptr<AST::Function> function, std::vector<std::unique_ptr<AST::Expression>> args): Expression(AST::get_type_by_name("__llvm_i32")), function(function), args(std::move(args)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "");
	};
}

namespace Parser {
	std::unique_ptr<AST::Function> parse_function(FILE* f);
	//std::unique_ptr<AST::FunctionPrototype> parse_function_prototype(bool is_inline = false);
}

#endif