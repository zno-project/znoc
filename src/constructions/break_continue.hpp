#ifndef _BREAK_CONTINUE_H
#define _BREAK_CONTINUE_H

#include "expression.hpp"
#include "../types/type.hpp"

#include <string>
#include <memory>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

namespace AST {
	class BreakExpression: public AST::Expression {
		private:
		std::unique_ptr<AST::Expression> value;
		
		public:
		BreakExpression(std::unique_ptr<AST::Expression> value): Expression(value ? value->getType() : AST::get_fundamental_type("void")), value(std::move(value)) {}
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
	};

	class ContinueExpression: public AST::Expression {
		public:
		ContinueExpression(): Expression(AST::get_fundamental_type("void")) {}
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
	};
}

#endif
