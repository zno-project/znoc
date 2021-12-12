#include "codeblock.hpp"

#include <vector>
#include <map>

#include "expression.hpp"
#include "../parsing.hpp"
#include "../macros.hpp"
#include "construction_parse.hpp"
#include "../memory/memory_location.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/IRBuilder.h>

#include <fmt/format.h>

std::vector<AST::CodeBlock*> codeblocks;

llvm::Value* AST::CodeBlock::codegen(llvm::IRBuilder<> *builder) {
	llvm::Value *returnExp = nullptr;
	for (auto &exp: body) {
		returnExp = exp->codegen(builder); // Set returned expression to last statement in block
	}

	return returnExp;
}

// CODE BLOCK
// codeblock = '{' statement* '}';
std::unique_ptr<AST::Expression> Parser::parse_code_block(FILE* f) {
	std::vector<std::unique_ptr<AST::Expression>> body;
	push_new_scope();

	EXPECT('{', "to start code block");
	UNTIL('}', {
		body.push_back(parse_statement(f));
	});

	auto c = std::make_unique<AST::CodeBlock>(std::move(body));
	c->push_before_return(pop_scope());
	return c;
}
