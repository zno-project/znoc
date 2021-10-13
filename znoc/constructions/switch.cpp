#include "switch.hpp"

#include <vector>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/IRBuilder.h>
#include "codeblock.hpp"
#include "../casting.hpp"
#include "../parsing.hpp"
#include "../main.hpp"
#include "../macros.hpp"
#include "binary_op.hpp"
#include <fmt/format.h>
#include <llvm/IR/BasicBlock.h>
#include "../types/builtins.hpp"
#include "construction_parse.hpp"

llvm::Value* AST::SwitchDef::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name) {
	//emitLocation(builder, this);
	llvm::Function *TheFunction = builder->GetInsertBlock()->getParent();
	llvm::BasicBlock *DefaultBB = llvm::BasicBlock::Create(*TheContext, "case_default");
	llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(*TheContext, "post_switch");

	auto sw_expr_code = switching_expr->codegen(builder);
	llvm::SwitchInst *sw = builder->CreateSwitch(sw_expr_code, DefaultBB, body.size());

	std::vector<llvm::BasicBlock*> bodyBlocks;

	for (auto &c: body) {
		llvm::BasicBlock *BodyBlock = llvm::BasicBlock::Create(*TheContext, fmt::format("case_{}", c.first.num[0]->getValue().getLimitedValue()));
		TheFunction->getBasicBlockList().push_back(BodyBlock);
		bodyBlocks.push_back(BodyBlock);
	}
	int i = 0;
	for (auto &c: body) {
		for (auto &n: c.first.num) {
			sw->addCase(n, bodyBlocks[i]);
		}
		builder->SetInsertPoint(bodyBlocks[i]);
		c.second->codegen(builder);
		if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(MergeBB);
		i++;
	}

	TheFunction->getBasicBlockList().push_back(DefaultBB);
	builder->SetInsertPoint(DefaultBB);
	if (default_body) default_body->codegen(builder);
	if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(MergeBB);

	TheFunction->getBasicBlockList().push_back(MergeBB);
	builder->SetInsertPoint(MergeBB);

	return nullptr;
}

// SWITCH STATEMENT
// switch = 'switch' binary_expr '{' switch_case* | 'default' codeblock '}';
std::unique_ptr<AST::Expression> Parser::parse_switch(FILE* f) {
	get_next_token(f); // Trim switch
	auto condition = parse_binary_expression(f);
	if (currentToken != '{') throw UNEXPECTED_CHAR(currentToken, "{ to start switch body");
	
	auto body = std::vector<std::pair<AST::SwitchDef::switch_case_metadata_t, std::unique_ptr<AST::CodeBlock>>>();
	std::unique_ptr<AST::CodeBlock> default_body;

	get_next_token(f); // Trim {
	if (currentToken != '}') while (1) {
		if (currentToken == tok_default) {
			get_next_token(f);
			if (currentToken != '{') throw UNEXPECTED_CHAR(currentToken, "{ to start switch default code block");
			default_body = std::unique_ptr<AST::CodeBlock>(static_cast<AST::CodeBlock*>(parse_code_block(f).release()));
		} else body.push_back(parse_switch_case(f));

		if (currentToken == '}') break;
	}
	get_next_token(f); // Trim }

	return std::make_unique<AST::SwitchDef>(std::move(condition), std::move(body), std::move(default_body));
}

// SWITCH CASE
// switch_case = 'case' (numeric_const ',')+ codeblock;
std::pair<AST::SwitchDef::switch_case_metadata_t, std::unique_ptr<AST::CodeBlock>> Parser::parse_switch_case(FILE* f) {
	AST::SwitchDef::switch_case_metadata_t meta;

	if (currentToken != tok_case) throw UNEXPECTED_CHAR(currentToken, "'case' inside switch statement");
	get_next_token(f);
	while (1) {
		if (currentToken != tok_numeric_literal) throw UNEXPECTED_CHAR(currentToken, "numeric literal for case value");
		int val = *std::get_if<double>(&currentTokenVal);
		meta.num.push_back(llvm::ConstantInt::get(llvm::IntegerType::get(*TheContext, 32), val));
		get_next_token(f);
		if (currentToken == '{') break;
		else if (currentToken != ',') throw UNEXPECTED_CHAR(currentToken, ", or { after case");
		get_next_token(f); // Trim ,
	}
	
	//if (currentToken != '{') throw UNEXPECTED_CHAR(currentToken, "{ to start case code block");
	std::unique_ptr<AST::CodeBlock> caseBody = std::unique_ptr<AST::CodeBlock>(static_cast<AST::CodeBlock*>(parse_code_block(f).release()));

	return std::pair<AST::SwitchDef::switch_case_metadata_t, std::unique_ptr<AST::CodeBlock>>(meta, std::move(caseBody));
}