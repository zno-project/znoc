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

llvm::Value* AST::CodeBlock::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string name) {
	//push_new_scope(); // Create new scope

	llvm::Value *returnExp = nullptr;
	for (auto &exp: body) {
		returnExp = exp->codegen(builder); // Set returned expression to last statement in block
	}

	//pop_scope(builder); // Close new scope
	return returnExp;
}

// CODE BLOCK
// codeblock = '{' statement* '}';
std::unique_ptr<AST::Expression> Parser::parse_code_block(FILE* f) {
	std::vector<std::unique_ptr<AST::Expression>> body;
	push_new_scope();

	//if (currentToken != '{') throw UNEXPECTED_CHAR(currentToken, "{ to start code block");
	//get_next_token(f);
	EXPECT('{', "to start code block");
	UNTIL('}', {
		body.push_back(parse_statement(f));
	});

	/*while (currentToken != '}') {
		body.push_back(parse_statement(f));
	}

	get_next_token(f); // Trim the }*/
	auto c = std::make_unique<AST::CodeBlock>(std::move(body));
	c->push_before_return(pop_scope());
	return c;
}
