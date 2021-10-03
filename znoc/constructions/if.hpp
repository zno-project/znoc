#ifndef _IF_H
#define _IF_H

#include "expression.hpp"
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

namespace AST {
	class IfDef: public AST::Expression {
		std::unique_ptr<AST::Expression> condition, thenClause, elseClause;

		public:
		IfDef(std::unique_ptr<AST::Expression> condition, std::unique_ptr<AST::Expression> thenClause, std::unique_ptr<AST::Expression> elseClause): Expression(thenClause->getType()), condition(std::move(condition)), thenClause(std::move(thenClause)), elseClause(std::move(elseClause)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string _name = "");
	};
}

namespace Parser {
	std::unique_ptr<AST::IfDef> parse_if_def(FILE* f);
}

#endif