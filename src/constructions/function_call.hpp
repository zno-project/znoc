#ifndef _FUNCTION_H
#define _FUNCTION_H

#include "expression.hpp"
#include "../types/type.hpp"
#include "codeblock.hpp"
#include "../memory/variable.hpp"
#include "../attributes.hpp"
#include "../mangling.hpp"

#include <map>
#include <string>
#include <vector>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>

#include "function_def.hpp"

namespace AST {
	class FunctionCall: public AST::Expression {
		private:
		std::shared_ptr<AST::Function> function;
		std::vector<std::unique_ptr<AST::Expression>> args;

		public:
		//FunctionCall(std::string callee, std::vector<std::unique_ptr<AST::Expression>> args): Expression(AST::get_type_by_name("__llvm_i32")), function(AST::get_function_by_name(callee)), args(std::move(args)) {}
		FunctionCall(std::shared_ptr<AST::Function> function, std::vector<std::unique_ptr<AST::Expression>> args): Expression(function->getRetType()), function(function), args(std::move(args)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "");
	};
}

#endif
