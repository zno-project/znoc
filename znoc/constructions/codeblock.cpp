#include "codeblock.hpp"

#include "expression.hpp"
#include "../parsing.hpp"
#include <vector>
#include <map>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/IRBuilder.h>
#include "reference.hpp"
#include "../macros.hpp"
#include "../main.hpp"
#include "../casting.hpp"
#include <fmt/format.h>

std::vector<AST::CodeBlock*> codeblocks;

llvm::Value* AST::CodeBlock::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string name) {
	NamedValues.push_front(std::map<std::string, std::shared_ptr<AST::Variable>>()); // Create new scope

	llvm::Value *returnExp = nullptr;
	for (auto &exp: body) {
		returnExp = exp->codegen(builder); // Set returned expression to last statement in block
	}

	NamedValues.pop_front(); // Close new scope
	return returnExp;
}

// CODE BLOCK
// codeblock = '{' statement* '}';
std::unique_ptr<AST::CodeBlock> Parser::parse_code_block(FILE* f) {
	std::vector<std::unique_ptr<AST::Expression>> body;

	if (currentToken != '{') throw UNEXPECTED_CHAR(currentToken, "{ to start code block");
	get_next_token(f);
	while (currentToken != '}') {
		body.push_back(parse_statement(f));
	}

	get_next_token(f); // Trim the }
	return std::make_unique<AST::CodeBlock>(std::move(body));
}