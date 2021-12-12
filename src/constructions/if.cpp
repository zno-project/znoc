#include "if.hpp"

#include "../llvm_module.hpp"
#include "../casting.hpp"
#include "../macros.hpp"
#include "../parsing.hpp"
#include "reference.hpp"
#include "../memory/memory_location.hpp"
#include "construction_parse.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Constant.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>

llvm::Value* AST::IfDef::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name) {
	auto conditionCond = condition->codegen(builder, "__if_condition");
	
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
std::unique_ptr<AST::Expression> Parser::parse_if_def(FILE* f) {
	EXPECT(tok_if, "to start if statement");

	push_new_scope();

	auto condition = parse_pratt_expression(f);

	std::unique_ptr<AST::CodeBlock> if_body = std::unique_ptr<AST::CodeBlock>(static_cast<AST::CodeBlock*>(parse_code_block(f).release()));
	if_body->push_before_return(pop_scope());

	IF_TOK_ELSE(tok_else, {
		std::unique_ptr<AST::Expression> else_body;
		IF_TOK_ELSE(tok_if, {
			else_body = parse_if_def(f);
		}, {
			else_body = parse_code_block(f);
		});
		return std::make_unique<AST::IfDef>(std::move(condition), std::move(if_body), std::move(else_body));
	}, {
		return std::make_unique<AST::IfDef>(std::move(condition), std::move(if_body), nullptr);
	});
}
