#ifndef _ZNO_PRATT_PARSER_H
#define _ZNO_PRATT_PARSER_H

#include "expression.hpp"
#include <memory>
#include <iostream>
#include "numeric_literal.hpp"

enum operators {
	not_an_operator = -1,

	dot,
	double_colon,

	plus,
	minus,
	star,
	divide,
	mod,

	ampersand,
	bitwise_or,
	bitwise_not,
	bitwise_xor,

	logical_and,
	logical_or,
	logical_not,

	shift_left,
	shift_right,

	compare_eq,
	compare_neq,
	compare_lt,
	compare_gt,
	compare_lte,
	compare_gte,

	assign,
	assign_add,
	assign_sub,
	assign_mul,
	assign_div,
	assign_mod,

	assign_bitwise_and,
	assign_bitwise_or,
	assign_bitwise_xor,
	assign_shift_left,
	assign_shift_right,
	
	is,
	subscript
};

extern char *operator_to_string[];

namespace AST {
	class UnaryExpressionPrefix: public Expression {
		private:
			operators op;
			std::unique_ptr<Expression> expr;
		public:
			UnaryExpressionPrefix(operators op, std::unique_ptr<Expression> expr) : op(op), expr(std::move(expr)), Expression(expr->getType()) {}
			virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "") { return nullptr; }
			
			virtual std::string print() const override {
				return std::string("(") + operator_to_string[op] + " " + expr->print() + ")";
			}
	};

	class UnaryExpressionPostfix: public Expression {
		private:
			operators op;
			std::unique_ptr<Expression> expr;
		public:
			UnaryExpressionPostfix(operators op, std::unique_ptr<Expression> expr) : op(op), expr(std::move(expr)), Expression(expr->getType()) {}
			virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "") { return nullptr; }
			
			virtual std::string print() const override {
				return std::string("(") + expr->print() + " " + operator_to_string[op] = ")";
			}
	};

	class NewBinaryExpression: public Expression {
		public:
			operators op;
			std::unique_ptr<Expression> lhs, rhs;
		public:
			NewBinaryExpression(operators op, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)), Expression(lhs->getType()) {}
			virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "") { return nullptr; }
			
			std::string print() const override {
				return std::string("(") + lhs->print() + " " + operator_to_string[op] + " " + rhs->print() + ")";
			}
	};
}

struct PrefixBindingPower {
	unsigned int rhs;
};

struct InfixBindingPower {
	unsigned int lhs, rhs;
};

struct PostfixBindingPower {
	unsigned int lhs;
};

#endif
