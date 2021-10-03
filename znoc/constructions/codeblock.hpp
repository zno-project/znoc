#ifndef _CODEBLOCK_H
#define _CODEBLOCK_H

#include "expression.hpp"
#include <vector>
#include <map>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>

namespace AST {
	class CodeBlock: public AST::Expression {
		std::vector<std::unique_ptr<AST::Expression>> body;

		public:
		CodeBlock(std::vector<std::unique_ptr<AST::Expression>> body): Expression(body.back()->getType()), body(std::move(body)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "");
	};
}

namespace Parser {
	std::unique_ptr<AST::CodeBlock> parse_code_block(FILE* f);
}

#endif