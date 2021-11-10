#include "binary_op.hpp"

#include "../parsing.hpp"
#include "reference.hpp"
#include "expression.hpp"
#include "../macros.hpp"
#include "unary_op.hpp"
#include "construction_parse.hpp"
#include "../memory/memory_ref.hpp"

#include <memory>
#include <stdio.h>
#include <iostream>
#include <string>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

#include <fmt/format.h>

llvm::Value* AST::BinaryExpression::codegen(llvm::IRBuilder<> *builder, std::string name) {
	llvm::Value *RHSv_value = RHS->codegen(builder, fmt::format("{}_RHS", name)); // Codegen the RHS

	if (op == op_assign) {
		/*ASTRef *LHS_variable_ref = dynamic_cast<ASTRef*>(LHS.get()); // Check if LHS is direct variable access
		if (!LHS_variable_ref) { // If not direct variable assignment
			// Check is dereference
			Expression *LHS_deref = LHS.get();
			while (auto LHS_unary = dynamic_cast<UnaryExpression*>(LHS_deref)) { // If dereference, check if unary op with `*` operator
				if (LHS_unary->op != '*') throw std::runtime_error(fmt::format("can only have `*` on LHS of assignment"));
				LHS_deref = LHS_unary->operand.get(); // Recursively descend through dereferences
			}
			LHS_variable_ref = dynamic_cast<ASTRef*>(LHS_deref);
			if (VariableRef *LHSv = dynamic_cast<VariableRef*>(LHS_variable_ref)) {
				builder->CreateStore(RHSv_value, LHSv->codegen(builder));
				return nullptr;
			} else throw std::runtime_error(fmt::format("1 assign op only for vars, found {} instead", typeid(*LHS_variable_ref).name()));
		} else if (VariableRef *LHSv = dynamic_cast<VariableRef*>(LHS.get())) {
			builder->CreateStore(RHSv_value, LHSv->getFieldAlloca(builder));
			return LHSv->codegen(builder);
		} else throw std::runtime_error(fmt::format("2 assign op only for vars, found {} instead", typeid(*LHS.get()).name()));*/
		Expression *LHS_deref = LHS.get();

		int deref_count = 0;
		while (auto LHS_unary = dynamic_cast<AST::UnaryExpression*>(LHS_deref)) { // If dereference, check if unary op with `*` operator
			if (LHS_unary->op != '*') throw std::runtime_error(fmt::format("can only have `*` unary op in l-value"));
			LHS_deref = LHS_unary->operand.get(); // Recursively descend through dereferences
			deref_count++;
		}

		if (auto LHS_variable_ref = dynamic_cast<AST::MemoryRef*>(LHS_deref)) {
			llvm::Value *final_alloca = LHS_variable_ref->codegen_to_underlying_ptr(builder);
			for (int i = 0; i < deref_count; i++) final_alloca = builder->CreateLoad(final_alloca);

			builder->CreateStore(RHSv_value, final_alloca);
			return LHS_variable_ref->codegen(builder);
		} else throw std::runtime_error("Can only deref an l-value");
	}

	llvm::Value *LHS_value = LHS->codegen(builder, fmt::format("{}_LHS", name)); // If not assignment, codegen LHS

	switch (op) {
		case op_add:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFAdd(LHS_value, RHSv_value, name);
			else return builder->CreateAdd(LHS_value, RHSv_value, name);
		case op_subtract:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFSub(LHS_value, RHSv_value, name);
			else return builder->CreateSub(LHS_value, RHSv_value, name);
		case op_multiply:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFMul(LHS_value, RHSv_value, name);
			else return builder->CreateMul(LHS_value, RHSv_value, name);
		case op_divide:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFDiv(LHS_value, RHSv_value, name);
			else return builder->CreateUDiv(LHS_value, RHSv_value, name);
		case op_modulo:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFRem(LHS_value, RHSv_value, name);
			else return builder->CreateURem(LHS_value, RHSv_value, name);

		case op_leftshift: return builder->CreateShl(LHS_value, RHSv_value, name);
		case op_rightshift: return builder->CreateAShr(LHS_value, RHSv_value, name);

		case op_bitwise_and: return builder->CreateAnd(LHS_value, RHSv_value, name);
		case op_bitwise_or: return builder->CreateOr(LHS_value, RHSv_value, name);

		case op_equal:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFCmpUEQ(LHS_value, RHSv_value, name);
			else return builder->CreateICmpEQ(LHS_value, RHSv_value, name);
		case op_not_equal:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFCmpUNE(LHS_value, RHSv_value, name);
			else return builder->CreateICmpNE(LHS_value, RHSv_value, name);
		case op_less_than:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFCmpULT(LHS_value, RHSv_value, name);
			else return builder->CreateICmpULT(LHS_value, RHSv_value, name);
		case op_less_than_equal:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFCmpULE(LHS_value, RHSv_value, name);
			else return builder->CreateICmpULE(LHS_value, RHSv_value, name);
		case op_greater_than:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFCmpUGT(LHS_value, RHSv_value, name);
			else return builder->CreateICmpUGT(LHS_value, RHSv_value, name);
		case op_greater_than_equal:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFCmpUGE(LHS_value, RHSv_value, name);
			else return builder->CreateICmpUGE(LHS_value, RHSv_value, name);

		default: throw std::runtime_error("unimplemented binary op");
	}
}

AST::OpType Parser::parse_binary_op(FILE* f) {
	AST::OpType ret;
	char op = currentToken;
	switch (op) {
		case '=':
			get_next_token(f);
			if (currentToken != '=') return AST::op_assign;
			get_next_token(f);
			return AST::op_equal;
		case '!':
			get_next_token(f);
			if (currentToken != '=') throw UNEXPECTED_CHAR(currentToken, "= to form !=");
			get_next_token(f);
			return AST::op_not_equal;
		case '<':
			get_next_token(f);
			if (currentToken == '<') {
				ret = AST::op_leftshift;
				get_next_token(f);
			} else if (currentToken == '=') {
				ret = AST::op_less_than_equal;
				get_next_token(f);
			} else {
				ret = AST::op_less_than;
			}
			return ret;
		case '>':
			get_next_token(f);
			if (currentToken == '>') {
				ret = AST::op_rightshift;
				get_next_token(f);
			} else if (currentToken == '=') {
				ret = AST::op_greater_than_equal;
				get_next_token(f);
			} else {
				ret = AST::op_greater_than;
			}
			return ret;
		case '|':
			get_next_token(f);
			if (currentToken != '|') return AST::op_bitwise_or;
			get_next_token(f);
			return AST::op_logical_or;
		case '&':
			get_next_token(f);
			if (currentToken != '&') return AST::op_bitwise_and;
			get_next_token(f);
			return AST::op_logical_and;
		case '+':
			get_next_token(f);
			return AST::op_add;
		case '-':
			get_next_token(f);
			return AST::op_subtract;
		case '*':
			get_next_token(f);
			return AST::op_multiply;
		case '/':
			get_next_token(f);
			return AST::op_divide;
		case '%':
			get_next_token(f);
			return AST::op_modulo;
		default: return AST::op_error;
	}
}

// BINARY EXPRESSION
// binary_expr = unary_expr (binary_op binary_expr)?;
std::unique_ptr<AST::Expression> Parser::parse_binary_expression(FILE* f) {
	auto LHS = parse_unary_expression(f);
	//std::cout << "bin expr - current tok is " << currentToken << std::endl;
	AST::OpType op = parse_binary_op(f);
	if (op < 0) return LHS; // If no `op + RHS` return the LHS on its own

	auto RHS = parse_binary_expression(f);
	return std::make_unique<AST::BinaryExpression>(op, std::move(LHS), std::move(RHS));
}
