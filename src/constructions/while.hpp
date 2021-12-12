#ifndef _WHILE_H
#define _WHILE_H

#include "expression.hpp"
#include "codeblock.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <string>

namespace AST {
	class WhileDef: public Expression {
		private:
		std::unique_ptr<AST::Expression> condition;
		std::unique_ptr<AST::CodeBlock> body;

		public:
		WhileDef(std::unique_ptr<AST::Expression> condition, std::unique_ptr<AST::CodeBlock> body): Expression(body->getType()), condition(std::move(condition)), body(std::move(body)) {}
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
	};
}

#endif
