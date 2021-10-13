#include "numeric_literal.hpp"
#include "../parsing.hpp"
#include "../types/builtins.hpp"
#include "construction_parse.hpp"

#include <llvm/IR/Constant.h>
#include <llvm/ADT/APFloat.h>
#include <cfloat>
#include <iostream>

llvm::Value* AST::NumericLiteral::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name) {
	//return llvm::ConstantInt::get(*TheContext, llvm::APInt(32, value)); //*/ return ConstantInt::get(IntegerType::get(*TheContext, 128), APInt(128, value, false));
	//return builder->CreateFPToUI(v, IntegerType::get(*TheContext, 32));
	if (is_int_val) {
		if (value.i > UINT32_MAX) return llvm::ConstantInt::get(*TheContext, llvm::APInt(64, value.i));
		else return llvm::ConstantInt::get(*TheContext, llvm::APInt(32, value.i));
	}
	auto v = llvm::ConstantFP::get(*TheContext, llvm::APFloat(value.f));
	if (value.f > FLT_MAX) return v;
	else return builder->CreateFPCast(v, AST::get_fundamental_type("float").codegen());
}

// NUMERIC LITERAL
// numeric_literal = NUMBER+;
std::unique_ptr<AST::Expression> Parser::parse_numeric_literal(FILE* f) {
	double val = *std::get_if<double>(&currentTokenVal);
	auto result = std::make_unique<AST::NumericLiteral>(val);
	get_next_token(f); // Move onto token after number
	return result;
}