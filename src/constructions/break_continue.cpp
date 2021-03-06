#include "break_continue.hpp"

#include <memory>
#include <string>

#include "expression.hpp"
#include "../parsing.hpp"
#include "construction_parse.hpp"
#include "../llvm_module.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>

llvm::Value* AST::BreakExpression::codegen(llvm::IRBuilder<> *builder) {
	auto v = value ? value->codegen(builder) : nullptr;
	if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(mergeBB); // Jump to the merge block
	return v;
}

llvm::Value* AST::ContinueExpression::codegen(llvm::IRBuilder<> *builder) {
	if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(condBB); // Jump to the condition block
	return llvm::UndefValue::get(llvm::Type::getVoidTy(*TheContext)); // Cannot return from a `continue`
}

// BREAK
// break = 'break' binary_expr?;
std::unique_ptr<AST::Expression> Parser::parse_break(zno_ifile& f) {
	EXPECT(tok_break, "in break statement");
	if (currentToken == ';') return std::make_unique<AST::BreakExpression>(nullptr);
	else return std::make_unique<AST::BreakExpression>(parse_pratt_expression(f));

	// NOTE: Can't use OPTIONAL or IF_TOK_ELSE since can't trim semicolon
}

std::unique_ptr<AST::Expression> Parser::parse_continue(zno_ifile& f) {
	EXPECT(tok_continue, "in continue statement");
	return std::make_unique<AST::ContinueExpression>();
}
