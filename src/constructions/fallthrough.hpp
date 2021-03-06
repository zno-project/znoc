#ifndef _FALLTHROUGH_H
#define _FALLTHROUGH_H

#include "expression.hpp"
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <string>
#include "../types/type.hpp"

namespace AST {
	class FallthroughExpression: public AST::Expression {
		public:
		FallthroughExpression(): Expression(AST::get_fundamental_type("void")) {}
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
	};
}

#endif
