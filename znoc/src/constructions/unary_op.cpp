#include "unary_op.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <fmt/format.h>
#include <iostream>
#include "../casting.hpp"
#include "../parsing.hpp"
#include "reference.hpp"
#include "expression.hpp"
#include "../macros.hpp"
#include "../types/builtins.hpp"
#include "../memory/memory_ref.hpp"
#include "construction_parse.hpp"

llvm::Value* AST::UnaryExpression::codegen(llvm::IRBuilder<> *builder, std::string name) {
	llvm::Value *OPv = operand->codegen(builder, fmt::format("{}_operand", name));

	switch (op) {
		case '!':
		case '~': return builder->CreateNot(OPv, name);
		case '*': return builder->CreateLoad(OPv);
		case '-': {
			if (OPv->getType()->isIntegerTy()) {
				return builder->CreateNeg(OPv);
			} else {
				return builder->CreateFNeg(OPv);
			}
		}
		case '&': {
			auto v = dynamic_cast<AST::MemoryRef*>(operand.get());
			if (!v) throw std::runtime_error("Cannot get pointer to non variable");
			return v->codegen_to_underlying_ptr(builder);
		}
		
		default: throw std::runtime_error(fmt::format("unimplemented unary op {}", op));
	}
}

// UNARY EXPRESSION
// unary_expr = r_value |
//              unary_op unary_expr;
std::unique_ptr<AST::Expression> Parser::parse_unary_expression(FILE* f) {
	if (!isascii(currentToken) || currentToken == '(' || currentToken == ',') return parse_r_value(f);

	char op = currentToken; // Trim the operator
	std::unique_ptr<AST::Expression> operand;

	get_next_token(f);
	operand = parse_unary_expression(f);

	return std::make_unique<AST::UnaryExpression>(op, std::move(operand));
}