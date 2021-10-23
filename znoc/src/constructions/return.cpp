#include "return.hpp"
#include "../casting.hpp"
#include "../parsing.hpp"
#include "../main.hpp"
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
	//emitLocation(builder, this);
	//llvm::Function *TheFunction = builder->GetInsertBlock()->getParent();
	//auto funct = NamedFunctions[TheFunction->getName().str()];
	if (value) {
		/*if (functMeta.is_throwing) {
			builder->CreateStore(create_cast(builder, value->codegen(builder, name), functMeta.return_ty), TheFunction->getArg(0));
			builder->CreateRet(llvm::ConstantInt::get(llvm::IntegerType::get(*TheContext, 32), 0));
		} else*/ if (!builder->GetInsertBlock()->getTerminator()) builder->CreateRet(value->codegen(builder, name));
	}
	//else if (functMeta.is_throwing) builder->CreateRet(llvm::ConstantInt::get(llvm::IntegerType::get(*TheContext, 32), 0));
	else if (!builder->GetInsertBlock()->getTerminator()) builder->CreateRetVoid();

	// Find current code block and set return val of that to be this
	/*if (value) codeblocks.back()->retVal = value->codegen(builder, name);
	else codeblocks.back()->retVal = nullptr;*/
	/*llvm::BasicBlock *retBlock = codeblocks.back()->endBlock;
	auto v = value ? value->codegen(builder, name) : nullptr;

	codeblocks.back()->retVals.push_back(std::pair<llvm::BasicBlock*, llvm::Value*>(builder->GetInsertBlock(), v));
	builder->CreateBr(retBlock);*/

	return nullptr;
}

std::unique_ptr<AST::Expression> Parser::parse_return(FILE* f) {
	EXPECT(tok_return, "in return statement")
	if (currentToken == ';') return std::make_unique<AST::ReturnExpression>(nullptr);
	else return std::make_unique<AST::ReturnExpression>(parse_binary_expression(f));

	// NOTE: Can't use OPTIONAL or IF_TOK_ELSE since can't trim semicolon
}