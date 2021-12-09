#ifndef _ZNO_STRING_H
#define _ZNO_STRING_H

#include "expression.hpp"
#include "construction_parse.hpp"
#include "../parsing.hpp"
#include "../macros.hpp"
#include "../types/type.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

namespace AST {
	class String: public Expression {
		private:
		std::string s;
		public:
		String(std::string s): Expression(AST::get_fundamental_type("i8").get_pointer_to()), s(std::move(s)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "");
	};
}

#endif
