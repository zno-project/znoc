#ifndef _VARIABLE_REF_H
#define _VARIABLE_REF_H

#include <string>
#include <map>
#include <deque>
#include <llvm/IR/IRBuilder.h>

#include "../types/type.hpp"
#include "memory_location.hpp"

namespace AST {
	class Variable: public AST::MemoryLoc {

		public:
		Variable(std::string name, AST::TypeInstance type): MemoryLoc(type, name) {}
		static std::shared_ptr<AST::Variable> create_in_scope(std::string name, AST::TypeInstance type);
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
		const std::string& getName() const {return name;}
	};

	std::shared_ptr<AST::MemoryLoc> get_var(std::string var);

	class GlobalVariable: public AST::MemoryLoc {
		private:
		std::string link_name;
		std::unique_ptr<AST::Expression> initializer;

		public:
		GlobalVariable(std::string name, std::string link_name, std::unique_ptr<AST::Expression> initializer): MemoryLoc(initializer->getType(), name), link_name(link_name), initializer(std::move(initializer)) {}
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
	};
}

#include "../constructions/expression.hpp"

namespace AST {
	class VariableDef: public Expression {
		private:
		std::shared_ptr<AST::Variable> var_ref;
		std::unique_ptr<Expression> value;

		public:
		VariableDef(std::string name, AST::TypeInstance type, std::unique_ptr<Expression> value): Expression(type), var_ref(AST::Variable::create_in_scope(name, type)), value(std::move(value)) {}
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
	};
}

namespace Parser {
	std::unique_ptr<AST::GlobalVariable> parse_global_variable_def(zno_ifile& f);
}

#endif
