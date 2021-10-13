#include "while.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/ADT/APFloat.h>
#include <string>
#include <iostream>
#include "../main.hpp"
#include "../casting.hpp"
#include "../macros.hpp"
#include "binary_op.hpp"
#include "../parsing.hpp"
#include "expression.hpp"
#include "construction_parse.hpp"

llvm::Value* AST::WhileDef::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name) {
	//emitLocation(builder, this);
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
	//auto condition = builder->CreateFCmpONE(conditionCond, llvm::ConstantFP::get(*TheContext, llvm::APFloat(0.0f)), "__while_condition_casted");
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
	get_next_token(f); // Trim while
	auto condition = parse_binary_expression(f);
	if (currentToken != '{') throw UNEXPECTED_CHAR(currentToken, "{ to start while body");
	
	std::unique_ptr<AST::CodeBlock> body = std::unique_ptr<AST::CodeBlock>(static_cast<AST::CodeBlock*>(parse_code_block(f).release()));

	return std::make_unique<AST::WhileDef>(std::move(condition), std::move(body));
}