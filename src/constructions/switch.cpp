#include "switch.hpp"

#include <vector>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/IRBuilder.h>
#include "codeblock.hpp"
#include "../parsing.hpp"
#include "../macros.hpp"
#include <fmt/format.h>
#include <llvm/IR/BasicBlock.h>
#include "../types/type.hpp"
#include "construction_parse.hpp"
#include "../llvm_module.hpp"
#include "../memory/memory_location.hpp"

llvm::Value* AST::SwitchDef::codegen(llvm::IRBuilder<> *builder) {
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

	return llvm::UndefValue::get(llvm::Type::getVoidTy(*TheContext));
}

// SWITCH STATEMENT
// switch = 'switch' binary_expr '{' switch_case* | 'default' codeblock '}';
std::unique_ptr<AST::Expression> Parser::parse_switch(zno_ifile& f) {
	EXPECT(tok_switch, "in switch statement");

	push_new_scope();
	auto condition = parse_pratt_expression(f);
	
	auto body = std::vector<std::pair<AST::SwitchDef::switch_case_metadata_t, std::unique_ptr<AST::CodeBlock>>>();
	std::unique_ptr<AST::CodeBlock> default_body;

	EXPECT('{', "to start switch body");
	UNTIL('}', {
		IF_TOK_ELSE(tok_default, {
			default_body = std::unique_ptr<AST::CodeBlock>(static_cast<AST::CodeBlock*>(parse_code_block(f).release()));
			default_body->push_before_return(pop_scope());
		}, {
			auto switch_case = parse_switch_case(f);
			switch_case.second->push_before_return(pop_scope());
			body.push_back(std::move(switch_case));
		});
	});

	return std::make_unique<AST::SwitchDef>(std::move(condition), std::move(body), std::move(default_body));
}

// SWITCH CASE
// switch_case = 'case' (numeric_const ',')+ codeblock;
std::pair<AST::SwitchDef::switch_case_metadata_t, std::unique_ptr<AST::CodeBlock>> Parser::parse_switch_case(zno_ifile& f) {
	AST::SwitchDef::switch_case_metadata_t meta;

	EXPECT(tok_case, "inside switch statement");

	while (1) {
		auto numeric_literal = EXPECT_NUMERIC_LITERAL("numeric literal for case value");
		int val = std::get<0>(numeric_literal);
		bool is_decimal = std::get<1>(numeric_literal);
		if (is_decimal) throw std::runtime_error("case values may only be integer literals");
		meta.num.push_back(llvm::ConstantInt::get(llvm::IntegerType::get(*TheContext, 32), val));
		if (currentToken == '{') break;
		EXPECT(',', "or { after case");
	}
	
	std::unique_ptr<AST::CodeBlock> caseBody = std::unique_ptr<AST::CodeBlock>(static_cast<AST::CodeBlock*>(parse_code_block(f).release()));

	return std::pair<AST::SwitchDef::switch_case_metadata_t, std::unique_ptr<AST::CodeBlock>>(meta, std::move(caseBody));
}
