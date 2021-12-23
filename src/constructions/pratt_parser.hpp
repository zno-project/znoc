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
	subscript,
	call
};

extern const char *operator_to_string[];

#include "function_def.hpp"

namespace AST {
	class UnaryExpressionPrefix: public Expression {
		private:
			AST::TypeInstance get_prefix_op_ret_type(operators op, std::unique_ptr<AST::Expression>& expr);
		public:
			operators op;
			std::unique_ptr<Expression> expr;
		public:
			UnaryExpressionPrefix(operators op, std::unique_ptr<Expression> expr) : Expression(get_prefix_op_ret_type(op, expr)), op(op), expr(std::move(expr)) {}
			llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
			llvm::Constant* codegen_const() override;
			
			std::string print() const override {
				return std::string("(") + operator_to_string[op] + " " + expr->print() + ")";
			}

			llvm::Value* codegen_to_ptr(llvm::IRBuilder<> *builder) override {
				if (op == operators::star) {
					return expr->codegen(builder);
				}
				return Expression::codegen_to_ptr(builder);
			}
	};

	class UnaryExpressionPostfix: public Expression {
		private:
			operators op;
			std::unique_ptr<Expression> expr;
		public:
			UnaryExpressionPostfix(operators op, std::unique_ptr<Expression> expr) : Expression(expr->getType()), op(op), expr(std::move(expr)) {}
			llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
			
			std::string print() const override {
				return std::string("(") + expr->print() + " " + operator_to_string[op] = ")";
			}
	};

	class NewCallExpression: public Expression {
		private:
			std::unique_ptr<Expression> func;
			std::vector<std::unique_ptr<Expression>> args;
		public:
			NewCallExpression(std::unique_ptr<Expression> func, std::vector<std::unique_ptr<Expression>> args) : Expression(func->getType().get_return_of_function()), func(std::move(func)), args(std::move(args)) {}
			llvm::Value* codegen(llvm::IRBuilder<> *builder) override;

			std::string print() const override {
				auto ret = std::string("(") + func->print() + "(";
				for (auto &arg : args) {
					ret += arg->print() + ", ";
				}
				ret += "))";
				return ret;
			}

			llvm::Value* codegen_to_ptr(llvm::IRBuilder<> *builder) override {
				auto val = codegen(builder);
				auto alloca = create_entry_block_alloca(builder->GetInsertBlock()->getParent(), "func_ret", val->getType());
				builder->CreateStore(val, alloca);
				return alloca;
			}
	};

	class NewBinaryExpression: public Expression {
		private:
			AST::TypeInstance get_binop_ret_type(operators op, std::unique_ptr<AST::Expression>& lhs, std::unique_ptr<AST::Expression>& rhs);
		public:
			operators op;
			std::unique_ptr<Expression> lhs, rhs;
		public:
			NewBinaryExpression(operators op, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) : Expression(get_binop_ret_type(op, lhs, rhs)), op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
			llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
			llvm::Constant* codegen_const() override;
			
			std::string print() const override {
				return std::string("(") + lhs->print() + " " + operator_to_string[op] + " " + rhs->print() + ")";
			}

			llvm::Value* codegen_to_ptr(llvm::IRBuilder<> *builder) override {
				if (op == subscript) {
					auto zero = llvm::ConstantInt::get(*TheContext, llvm::APInt(32, 0));
					auto i = rhs->codegen(builder);
					llvm::Value* a[] = {zero, i};
					auto idxs = llvm::ArrayRef<llvm::Value*>(a);

					auto gep = builder->CreateGEP(lhs->codegen_to_ptr(builder), idxs);
					return gep;
				}
				
				return rhs->codegen_to_ptr(builder);
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
