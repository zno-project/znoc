#include "numeric_literal.hpp"
#include "../parsing.hpp"
#include "../types/builtins.hpp"
#include "construction_parse.hpp"
#include "../macros.hpp"

#include <llvm/IR/Constant.h>
#include <llvm/ADT/APFloat.h>
#include <cfloat>
#include <iostream>
#include <memory>
#include <tuple>

llvm::Value* AST::NumericLiteral::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name) {
	//return llvm::ConstantInt::get(*TheContext, llvm::APInt(32, value)); //*/ return ConstantInt::get(IntegerType::get(*TheContext, 128), APInt(128, value, false));
	//return builder->CreateFPToUI(v, IntegerType::get(*TheContext, 32));
	if (is_int_val) {
		return llvm::ConstantInt::get(expressionType.codegen(), value.i);
	}
	auto v = llvm::ConstantFP::get(*TheContext, llvm::APFloat(value.f));
	return builder->CreateFPCast(v, expressionType.codegen());
}

// NUMERIC LITERAL
// numeric_literal = NUMBER+;
std::unique_ptr<AST::Expression> Parser::parse_numeric_literal(FILE* f) {
	//std::cout << "parse numlit" << std::endl;
	auto [ val, contains_dp ] = EXPECT_NUMBERIC_LITERAL("to start numeric literal");/*std::get<double>(currentTokenVal);
	get_next_token(f); // Move onto token after number*/

	IF_TOK_ELSE_IDENTIFIER(post_num_modifier, {
		//std::cout << "post num modifier: " << post_num_modifier << std::endl;
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

	/*if (currentToken == tok_identifier) {
		auto post_num_modifier = std::get<std::string>(currentTokenVal);
		get_next_token(f); // Trim modifier

		auto modifier_str = post_num_modifier.c_str();
		auto num_type_char = *modifier_str++;
		if (num_type_char != 'u' && num_type_char != 'f') throw UNEXPECTED_CHAR(num_type_char, "`u` or `f` qualifier after number");

		auto len = atoi(modifier_str);
		return num_type_char == 'u' ? AST::NumericLiteral::NewInt(val, len) : AST::NumericLiteral::NewFloat(val, len);
	} else {
		return std::make_unique<AST::NumericLiteral>(val);
	}*/
}