#ifndef _CASTING_H
#define _CASTING_H

#include "../constructions/expression.hpp"
#include "type.hpp"

namespace AST {
	class Cast: public AST::Expression {
		std::unique_ptr<AST::Expression> from;
		AST::TypeInstance to;

		public:
		Cast(std::unique_ptr<AST::Expression> from, AST::TypeInstance to): Expression(to), from(std::move(from)), to(to) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "");
	};

	std::unique_ptr<AST::Cast> make_cast(std::unique_ptr<AST::Expression> from, AST::TypeInstance to);
}

#endif