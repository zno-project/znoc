#include "fallthrough.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>
#include "../parsing.hpp"
#include "construction_parse.hpp"
#include "../llvm_module.hpp"

// FALLTHROUGH
// fallthrough = 'fallthrough';
llvm::Value* AST::FallthroughExpression::codegen(llvm::IRBuilder<> *builder) {
	llvm::BasicBlock *TheBlock = builder->GetInsertBlock();
	auto bbList = &(TheBlock->getParent()->getBasicBlockList()); // Get list of blocks in parent
	bool foundIt = false;
	llvm::BasicBlock *b = nullptr;
	for(auto bb = bbList->begin(); bb != bbList->end(); bb++) {
		b = &(*bb);
		if (foundIt) break;
		if(b->getName() == TheBlock->getName()) foundIt = true; // If block matches the currently being generated block, get the next block
	}

	if (!b) throw std::runtime_error("this really wasn't supposed to happen. or maybe you put a fallthrough with nothing following it or something");
	if (!builder->GetInsertBlock()->getTerminator()) builder->CreateBr(b);

	return llvm::UndefValue::get(llvm::Type::getVoidTy(*TheContext));
}

std::unique_ptr<AST::Expression> Parser::parse_fallthrough(zno_ifile& f) {
	get_next_token(f);
	return std::make_unique<AST::FallthroughExpression>();
}
