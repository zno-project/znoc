#include "if.hpp"

#include "../main.hpp"
#include "../casting.hpp"
#include "../macros.hpp"
#include "../parsing.hpp"
#include "reference.hpp"
#include "codeblock.hpp"
#include "binary_op.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Constant.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>

llvm::Value* AST::IfDef::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name) {
	//emitLocation(builder, this);
	NamedValues.push_front(std::map<std::string, std::shared_ptr<AST::Variable>>()); // Create new scope
	auto conditionCond = condition->codegen(builder, "__if_condition");
	//auto condition = builder->CreateFCmpONE(conditionCond, llvm::ConstantFP::get(*TheContext, llvm::APFloat(0.0f)), "__if_condition_casted");

	llvm::Function *TheFunction = builder->GetInsertBlock()->getParent();
	llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(*TheContext, "then", TheFunction);
	llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(*TheContext, "else");
	llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(*TheContext, "post_if");

	builder->CreateCondBr(conditionCond, ThenBB, ElseBB);

	builder->SetInsertPoint(ThenBB); // Start putting IR in if body
	auto thenBlockReturn = thenClause->codegen(builder);
	auto postThenBlockBlock = builder->GetInsertBlock();
	if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(MergeBB);

	TheFunction->getBasicBlockList().push_back(ElseBB); // Create else block
	builder->SetInsertPoint(ElseBB);
	llvm::Value *elseBlockReturn = nullptr;
	llvm::BasicBlock *postElseBlockBlock;
	if (elseClause) {
		elseBlockReturn = elseClause->codegen(builder);
		postElseBlockBlock = builder->GetInsertBlock();
	}
	if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(MergeBB);

	NamedValues.pop_front(); // Remove scope of condition

	TheFunction->getBasicBlockList().push_back(MergeBB);
	builder->SetInsertPoint(MergeBB);

	if (elseClause) {
		auto phi = builder->CreatePHI(thenBlockReturn->getType(), 2);
		phi->addIncoming(thenBlockReturn, postThenBlockBlock);
		phi->addIncoming(elseBlockReturn, postElseBlockBlock);
		return phi;
	}

	return thenBlockReturn;
}

// IF STATEMENT
// if = 'if' binary_expr codeblock ('else' (codeblock | if))?;
std::unique_ptr<AST::IfDef> Parser::parse_if_def(FILE* f) {
	get_next_token(f); // Trim if
	auto condition = parse_binary_expression(f);

	if (currentToken != '{') throw UNEXPECTED_CHAR(currentToken, "{ to start if code block");
	std::unique_ptr<AST::Expression> ifBody = parse_code_block(f);

	if (currentToken == tok_else) {
		get_next_token(f);
		std::unique_ptr<AST::Expression> elseBody;
		if (currentToken == tok_if) elseBody = parse_if_def(f);
		else if (currentToken == '{') {
			elseBody = parse_code_block(f);
		} else {
			throw UNEXPECTED_CHAR(currentToken, "{ to start if code block");
		}
		return std::make_unique<AST::IfDef>(std::move(condition), std::move(ifBody), std::move(elseBody));
	}

	return std::make_unique<AST::IfDef>(std::move(condition), std::move(ifBody), nullptr);
}