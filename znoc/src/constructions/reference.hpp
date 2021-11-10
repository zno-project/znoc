#ifndef _REFERENCE_H
#define _REFERENCE_H

#include "expression.hpp"
#include "../types/type.hpp"
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/IRBuilder.h>
#include <string>
#include <deque>
#include <map>

/*namespace AST {
	class ASTRef: public AST::Expression {
		public:
		ASTRef(AST::TypeInstance type) : Expression(type) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "") = 0;
		virtual ~ASTRef() = default;
		virtual const std::string& get_name() = 0;
	};
}*/

#endif
