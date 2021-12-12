#include "numeric_literal.hpp"
#include "../parsing.hpp"
#include "construction_parse.hpp"
#include "../macros.hpp"

#include <llvm/IR/Constant.h>
#include <llvm/ADT/APFloat.h>
#include <cfloat>
#include <iostream>
#include <memory>
#include <tuple>

llvm::Value* AST::NumericLiteral::codegen(llvm::IRBuilder<> *builder) {
	if (is_int_val) {
		return llvm::ConstantInt::get(expressionType.codegen(), value.i);
	}
	auto v = llvm::ConstantFP::get(*TheContext, llvm::APFloat(value.f));
	return builder->CreateFPCast(v, expressionType.codegen());
}

// NUMERIC LITERAL
// numeric_literal = NUMBER+;
std::unique_ptr<AST::Expression> Parser::parse_numeric_literal(FILE* f) {
	auto [ val, contains_dp ] = EXPECT_NUMERIC_LITERAL("to start numeric literal");

	IF_TOK_ELSE_IDENTIFIER(post_num_modifier, {
		auto modifier_str = post_num_modifier.c_str();
		auto num_type_char = *(modifier_str++);
		
		if (num_type_char != 'u' && num_type_char != 'i' && num_type_char != 'f' ) {
			throw UNEXPECTED_CHAR(num_type_char, "`u` or `i` or `f` qualifier after number");
		}
		auto len = atoi(modifier_str);
		return num_type_char == 'u' ? AST::NumericLiteral::NewUInt(val, len) : 
		       num_type_char == 'i' ? AST::NumericLiteral::NewInt(val, len) :
			                          AST::NumericLiteral::NewFloat(val, len);
	}, {
		return std::make_unique<AST::NumericLiteral>(val, contains_dp);
	});
}
