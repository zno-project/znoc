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
#include "../memory/variable.hpp"
#include <iostream>
#include "namespace.hpp"

namespace AST {
	class Reference: public AST::Expression {
		public:
		std::shared_ptr<AST::MemoryLoc> var;
		std::shared_ptr<AST::Namespace> ns;
		enum class T { ns, var, none } type;
		std::string name;
		public:
		/*Reference(std::string name) : var(AST::get_var(name)), name(name), Expression(({auto v = AST::get_var(name); v ? v->underlying_type : AST::get_fundamental_type("void");})) {
			std::cout << "reference to " << name << std::endl;
		}*/

		Reference(std::string name): Expression(AST::get_fundamental_type("void")) {
			this->type = T::none;
			this->name = name;
		}

		Reference(std::shared_ptr<AST::Namespace> ns, std::string name): Expression(AST::get_fundamental_type("namespace_ref")) {
			this->type = T::ns;
			this->ns = ns;
			this->name = name;
		}

		Reference(std::shared_ptr<AST::MemoryLoc> var, std::string name): Expression(var->underlying_type) {
			this->type = T::var;
			this->var = var;
			this->name = name;
		}

		void add_var(std::shared_ptr<AST::MemoryLoc> var) {
			this->var = var;
			this->type = T::var;
			this->expressionType = var->underlying_type;
		}

		void add_namespace(std::shared_ptr<AST::Namespace> ns) {
			this->ns = ns;
			this->type = T::ns;
			this->expressionType = AST::get_fundamental_type("namespace_ref");
		}

		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string _name = "") override {
			if (this->type == T::var) {
				auto codegenned_var = var->codegen(builder);
				if (codegenned_var->getType()->getPointerElementType()->isFunctionTy()) return codegenned_var;
				else return builder->CreateLoad(var->codegen(builder));
			}
			else return (llvm::Value*)&*ns;
		}

		virtual llvm::Value* codegen_to_ptr(llvm::IRBuilder<> *builder) override {
			assert(this->type == T::var);
			return var->codegen(builder);
		}

		std::shared_ptr<AST::MemoryLoc> get_var() {
			std::cout << "ref::get_var" << std::endl;
			return var;
		}

		std::string get_name() {
			return name;
		}
	};
}

#endif
