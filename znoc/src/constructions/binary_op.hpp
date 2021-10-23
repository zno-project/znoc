#ifndef _BINARY_OP_H
#define _BINARY_OP_H

#include "expression.hpp"

#include <memory>
#include <string>
#include <stdio.h>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

namespace AST {
	typedef enum {
			op_assign,

			op_less_than,
			op_less_than_equal,
			op_equal,
			op_not_equal,
			op_greater_than,
			op_greater_than_equal,

			op_logical_and,
			op_logical_or,

			op_bitwise_and,
			op_bitwise_or,

			op_leftshift,
			op_rightshift,

			op_add,
			op_subtract,
			op_multiply,
			op_divide,
			op_modulo,

			op_error = -1
	} OpType;

	class BinaryExpression: public AST::Expression {
		OpType op;
		std::unique_ptr<AST::Expression> LHS, RHS;

		public:
		BinaryExpression(OpType op, std::unique_ptr<AST::Expression> LHS, std::unique_ptr<AST::Expression> RHS): Expression(LHS->getType()), op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "");
	};
}

namespace Parser {
	AST::OpType parse_binary_op(FILE* f);
}

#endif