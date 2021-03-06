#ifndef _RETURN_H
#define _RETURN_H

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

#include "expression.hpp"
#include "../types/type.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>

#include <string>

namespace AST {
	class ReturnExpression: public Expression {
		private:
		std::unique_ptr<Expression> value;
		
		public:
		ReturnExpression(std::unique_ptr<Expression> value): Expression(value ? value->getType() : AST::get_fundamental_type("void")), value(std::move(value)) {}
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
	};
}

#endif
