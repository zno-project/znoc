#include "break_continue.hpp"
#include "expression.hpp"
#include "binary_op.hpp"
#include "../parsing.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>

llvm::Value* AST::BreakExpression::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string name) {
	auto v = value ? value->codegen(builder) : nullptr;
	if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(mergeBB); // Jump to the merge block
	return v;
}

llvm::Value* AST::ContinueExpression::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string name) {
	if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(condBB); // Jump to the condition block
	return nullptr; // Cannot return from a `continue`
}

// BREAK
// break = 'break' binary_expr?;
std::unique_ptr<AST::Expression> Parser::parse_break(FILE* f) {
	if (get_next_token(f) == ';') return std::make_unique<AST::BreakExpression>(nullptr);
	else return std::make_unique<AST::BreakExpression>(parse_binary_expression(f));
}

std::unique_ptr<AST::Expression> Parser::parse_continue(FILE* f) {
	get_next_token(f);
	return std::make_unique<AST::ContinueExpression>();
}