#ifndef _CODEBLOCK_H
#define _CODEBLOCK_H

#include "expression.hpp"

#include <vector>
#include <map>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

namespace AST {
	class CodeBlock: public AST::Expression {
		std::vector<std::unique_ptr<AST::Expression>> body;

		public:
		CodeBlock(std::vector<std::unique_ptr<AST::Expression>> body): Expression(body.back()->getType()), body(std::move(body)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "");
	};
}

#endif