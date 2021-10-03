#ifndef _BREAK_CONTINUE_H
#define _BREAK_CONTINUE_H

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

#include "expression.hpp"
#include "../types/type.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>

#include <string>

namespace AST {
	class BreakExpression: public AST::Expression {
		std::unique_ptr<AST::Expression> value;
		
		public:
		BreakExpression(std::unique_ptr<AST::Expression> value): Expression(value ? value->getType() : AST::get_type_by_name("__llvm_void")), value(std::move(value)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "retVal");
	};

	class ContinueExpression: public AST::Expression {
		public:
		ContinueExpression(): Expression(AST::get_type_by_name("__llvm_void")) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "retVal");
	};
}

namespace Parser {
	std::unique_ptr<AST::Expression> parse_continue(FILE* f);
	std::unique_ptr<AST::Expression> parse_break(FILE* f);
}

#endif