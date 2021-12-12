#include "return.hpp"
#include "../parsing.hpp"
#include "construction_parse.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>

#include <string>
#include <map>

// RETURN
// return = 'return' binary_expr?;
llvm::Value* AST::ReturnExpression::codegen(llvm::IRBuilder<> *builder, std::string name) {
	if (value) {
		if (!builder->GetInsertBlock()->getTerminator()) builder->CreateRet(value->codegen(builder, name));
	}
	else if (!builder->GetInsertBlock()->getTerminator()) builder->CreateRetVoid();

	return nullptr;
}

std::unique_ptr<AST::Expression> Parser::parse_return(FILE* f) {
	EXPECT(tok_return, "in return statement")
	if (currentToken == ';') return std::make_unique<AST::ReturnExpression>(nullptr);
	else return std::make_unique<AST::ReturnExpression>(parse_pratt_expression(f));

	// NOTE: Can't use OPTIONAL or IF_TOK_ELSE since can't trim semicolon
}