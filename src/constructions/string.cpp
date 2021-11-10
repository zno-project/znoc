#include "string.hpp"
#include "../llvm_module.hpp"

#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Constants.h>
#include <fmt/format.h>

uint64_t stringCounter = 0;

llvm::Value* AST::String::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string name) {
	auto str = llvm::ConstantDataArray::getString(*TheContext, llvm::StringRef(s), true);
	auto globalVar = new llvm::GlobalVariable(*TheModule, str->getType(), true, llvm::GlobalValue::PrivateLinkage, str, fmt::format("str.{}", stringCounter++));
	return builder->CreateBitCast(globalVar, AST::get_fundamental_type("i8").get_pointer_to().codegen());
}

std::unique_ptr<AST::Expression> Parser::parse_string(FILE* f) {
	if (currentToken != tok_string) UNEXPECTED_CHAR(currentToken, "string to begin string literal");
	std::string s = std::get<std::string>(currentTokenVal);
	get_next_token(f);
	return std::make_unique<AST::String>(s);
}
