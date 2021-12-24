#include "variable.hpp"
#include "../constructions/construction_parse.hpp"
#include "../parsing.hpp"
#include "../macros.hpp"
#include "../llvm_module.hpp"
#include "../main.hpp"

#include <llvm/IR/Module.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DataLayout.h>

#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <memory>
#include <optional>

llvm::AllocaInst* create_entry_block_alloca(llvm::Function *in_func, std::string name, llvm::Type* t) {
	llvm::IRBuilder<> tmpBuilder(&in_func->getEntryBlock(), in_func->getEntryBlock().begin());
	return tmpBuilder.CreateAlloca(t, nullptr, fmt::format("{}_stack", name));
}

llvm::Value* AST::Variable::codegen(llvm::IRBuilder<> *builder) {
	if (!allocaV) {
		llvm::Type *ty = underlying_type.codegen();
		allocaV = create_entry_block_alloca(builder->GetInsertBlock()->getParent(), name, ty);
	}
	return allocaV;
}

llvm::Value* AST::GlobalVariable::codegen(__attribute__((unused)) llvm::IRBuilder<> *builder) {
	if (!allocaV) {
		allocaV = new llvm::GlobalVariable(*TheModule, underlying_type.codegen(), true, llvm::GlobalValue::PrivateLinkage, initializer->codegen_const(), link_name);
	}
	return allocaV;
}

llvm::Value* AST::VariableDef::codegen(llvm::IRBuilder<> *builder) {
	auto allocaV = var_ref->codegen(builder);
	
	builder->CreateLifetimeStart(allocaV);
	if (value) {
		auto v = value->codegen(builder);
		builder->CreateStore(v, allocaV);
	}
	
	return builder->CreateLoad(var_ref->underlying_type.codegen(), allocaV);
}

llvm::Constant* AST::GlobalVariable::codegen_const_initialiser() {
	return initializer->codegen_const();
}

// VARIABLE DEFINITION
// variable_def = 'let' identifier (':' type)? ('=' binary_expr)?;
std::unique_ptr<AST::Expression> Parser::parse_variable_def(zno_ifile& f) {
	EXPECT(tok_let, "for variable definition");
	
	std::string name = EXPECT_IDENTIFIER("variable name");

	std::optional<AST::TypeInstance> type;

	OPTIONAL(':', {
		type = parse_type(f);
		if (currentToken != '=') return std::make_unique<AST::VariableDef>(name, std::move(*type), nullptr);
	});

	EXPECT('=', " or type after variable name");

	std::unique_ptr<AST::Expression> val = parse_pratt_expression(f);
	if (!type) type = val->getType();
	return std::make_unique<AST::VariableDef>(name, std::move(*type), std::move(val));
}

std::unique_ptr<AST::GlobalVariable> Parser::parse_global_variable_def(zno_ifile& f) {
	EXPECT(tok_let, "for variable definition");
	
	std::string name = EXPECT_IDENTIFIER("variable name");

	std::optional<AST::TypeInstance> type;

	/*OPTIONAL(':', {
		type = parse_type(f);
	});*/

	EXPECT('=', "initialiser for global variable");

	std::unique_ptr<AST::Expression> val = parse_pratt_expression(f);

	EXPECT(';', "after global variable definition");
	if (!type) type = val->getType();
	return std::make_unique<AST::GlobalVariable>(name, name, std::move(val));
}

#include "../constructions/namespace.hpp"

std::shared_ptr<AST::MemoryLoc> AST::get_var(std::string var) {
	for (auto &scope_vars: stack_allocations) {
		if (scope_vars.find(var) != scope_vars.end()) {
			auto m = scope_vars[var];
			auto v = std::dynamic_pointer_cast<AST::Variable>(m);
			if (!v) throw std::runtime_error("ummmmm... what?");
			return v;
		}
	}

	for (auto i = NamespaceStack.rbegin(); i != NamespaceStack.rend(); ++i) {
		auto m = (*i)->get_var(var);
		if (m) return m;
	}

	return nullptr;
}

std::shared_ptr<AST::Variable> AST::Variable::create_in_scope(std::string name, AST::TypeInstance type) {
	if (AST::get_var(name)) std::cerr << "WARNING: Redeclaration of variable " << name << std::endl;
	auto v = std::make_shared<AST::Variable>(name, type);
	stack_allocations.front()[name] = v;
	return v;
}
