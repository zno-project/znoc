#ifndef _RETURN_H
#define _RETURN_H

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

#include "expression.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>

#include <string>

namespace AST {
	class ReturnExpression: public Expression {
		std::unique_ptr<Expression> value;
		
		public:
		ReturnExpression(std::unique_ptr<Expression> value): Expression(value ? value->getType() : AST::get_type_by_name("__llvm_void")), value(std::move(value)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "retVal");
	};
}

namespace Parser {
	std::unique_ptr<AST::Expression> parse_return(FILE* f);
}

#endif