#ifndef _UNARY_OP_H
#define _UNARY_OP_H

#include "expression.hpp"
#include "reference.hpp"
#include "../types/builtins.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

namespace AST {
	/*typedef enum {
			op_not,

			op_error = -1
	} UnaryOpType;*/

	class UnaryExpression: public Expression {
		public:
		char op;
		std::unique_ptr<Expression> operand;
		UnaryExpression(char op, std::unique_ptr<Expression> operand): Expression(op == '&' ? AST::get_fundamental_type("ptr") : (op == '*' ? AST::get_fundamental_type("i32") : operand->getType())), op(op), operand(std::move(operand)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "");
	};
}

#endif