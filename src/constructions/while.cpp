#include "while.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/ADT/APFloat.h>
#include <string>
#include <iostream>
#include "../llvm_module.hpp"
#include "../casting.hpp"
#include "../macros.hpp"
#include "../parsing.hpp"
#include "expression.hpp"
#include "construction_parse.hpp"

llvm::Value* AST::WhileDef::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name) {
	llvm::Function *TheFunction = builder->GetInsertBlock()->getParent();
	condBB = llvm::BasicBlock::Create(*TheContext, "while_loop_condition");
	llvm::BasicBlock *WhileLoopBodyBB = llvm::BasicBlock::Create(*TheContext, "while_loop_body");
	mergeBB = llvm::BasicBlock::Create(*TheContext, "post_while");

	TheFunction->getBasicBlockList().push_back(condBB);
	TheFunction->getBasicBlockList().push_back(WhileLoopBodyBB);
	TheFunction->getBasicBlockList().push_back(mergeBB);

	if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(condBB);
	builder->SetInsertPoint(condBB);
	auto conditionCond = condition->codegen(builder, "__while_condition");
	builder->CreateCondBr(conditionCond, WhileLoopBodyBB, mergeBB);

	builder->SetInsertPoint(WhileLoopBodyBB);
	auto whileRetVal = body->codegen(builder);
	if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(condBB);

	builder->SetInsertPoint(mergeBB);

	mergeBB = nullptr;
	condBB = nullptr;

	return whileRetVal;
}

// WHILE LOOP
// while_loop = 'while' binary_expr codeblock;
std::unique_ptr<AST::Expression> Parser::parse_while(FILE* f) {
	EXPECT(tok_while, "to start while loop");
	push_new_scope();
	auto condition = parse_pratt_expression(f);
	
	std::unique_ptr<AST::CodeBlock> body = std::unique_ptr<AST::CodeBlock>(static_cast<AST::CodeBlock*>(parse_code_block(f).release()));
	body->push_before_return(pop_scope());

	return std::make_unique<AST::WhileDef>(std::move(condition), std::move(body));
}
