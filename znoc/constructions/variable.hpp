#ifndef _VARIABLE_REF_H
#define _VARIABLE_REF_H

#include "expression.hpp"
#include "../types/type.hpp"
#include <string>
#include <map>
#include <deque>
#include <llvm/IR/IRBuilder.h>

namespace AST {
	class Variable {
		std::string name;
		AST::type_usage_t type;
		llvm::Value *allocaV;

		public:
		Variable(std::string name, AST::type_usage_t type): name(name), type(type), allocaV(nullptr) {}
		static std::shared_ptr<AST::Variable> create(std::string name, AST::type_usage_t type);
		AST::type_usage_t getVarType() { return type; }
		llvm::Value* getAlloca() { return allocaV; }
		llvm::Value* codegen(llvm::IRBuilder<> *builder);
		const std::string& getName() const {return name;}
	};

	std::shared_ptr<AST::Variable> get_var(std::string var);

	class VariableDef: public Expression {
		std::shared_ptr<AST::Variable> varRef;
		std::unique_ptr<Expression> value;

		public:
		VariableDef(std::string name, AST::type_usage_t type, std::unique_ptr<Expression> value): Expression(type), varRef(Variable::create(name, type)), value(std::move(value)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name = "");
	};
}

extern std::deque<std::map<std::string, std::shared_ptr<AST::Variable>>> NamedValues;

#include "reference.hpp"

namespace AST {
	class VariableRef: public ASTRef {
		std::shared_ptr<AST::Variable> varRef;
		std::vector<std::string> fields;

		public:
		VariableRef(std::string var, std::vector<std::string> fields): ASTRef(AST::get_var(var)->getVarType()), varRef(AST::get_var(var)), fields(fields) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "");
		llvm::Value* getAlloca() { return varRef->getAlloca(); }
		llvm::Value* getFieldAlloca(llvm::IRBuilder<> *builder);
		virtual const std::string& get_name() { return varRef->getName(); }
	};
}

namespace Parser {
	std::unique_ptr<AST::Expression> parse_variable_def(FILE* f);
}

#endif