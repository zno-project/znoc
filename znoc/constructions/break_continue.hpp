#ifndef _BREAK_CONTINUE_H
#define _BREAK_CONTINUE_H

#include "expression.hpp"
#include "../types/type.hpp"
#include "../types/builtins.hpp"

#include <string>
#include <memory>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

namespace AST {
	class BreakExpression: public AST::Expression {
		std::unique_ptr<AST::Expression> value;
		
		public:
		BreakExpression(std::unique_ptr<AST::Expression> value): Expression(value ? value->getType() : AST::get_fundamental_type("void")), value(std::move(value)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "retVal");
	};

	class ContinueExpression: public AST::Expression {
		public:
		ContinueExpression(): Expression(AST::get_fundamental_type("void")) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "retVal");
	};
}

#endif