#ifndef _CODEBLOCK_H
#define _CODEBLOCK_H

#include "expression.hpp"

#include <vector>
#include <map>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

#include <iostream>

namespace AST {
	class CodeBlock: public AST::Expression {
		private:
		std::vector<std::unique_ptr<AST::Expression>> body;

		public:
		CodeBlock(std::vector<std::unique_ptr<AST::Expression>> body): Expression(body.back()->getType()), body(std::move(body)) {}
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
		llvm::Constant* codegen_const() override {
			std::cerr << "codeblock constexpr does not yet evaluate side effects" << std::endl;
			return body.back()->codegen_const();
		}
		void push_before_return(std::unique_ptr<AST::Expression> expr) {
			body.insert(body.end()-1, std::move(expr));
		}
	};
}

#endif
