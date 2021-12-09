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
	std::string type_str_src;
	llvm::raw_string_ostream rso(type_str_src);
	t->print(rso);

	//std::cout << "Creating entry block alloca for " << name << ": " << type_str_src << std::endl;


	llvm::IRBuilder<> tmpBuilder(&in_func->getEntryBlock(), in_func->getEntryBlock().begin());
	return tmpBuilder.CreateAlloca(t, nullptr, fmt::format("{}_stack", name));
}

//std::deque<std::map<std::string, std::shared_ptr<AST::Variable>>> NamedValues; // Deque of scopes, containing a map of variables in scope

llvm::Value* AST::Variable::codegen(llvm::IRBuilder<> *builder) {
	if (!allocaV) {
		llvm::Type *ty = underlying_type.codegen();
		allocaV = create_entry_block_alloca(builder->GetInsertBlock()->getParent(), name, ty);
	}
	return allocaV;
}

llvm::Value* AST::GlobalVariable::codegen(__attribute__((unused)) llvm::IRBuilder<> *builder) {
	if (!allocaV) {
		allocaV = new llvm::GlobalVariable(*TheModule, underlying_type.codegen(), true, llvm::GlobalValue::PrivateLinkage, initializer->codegen_const(), name);
		//llvm::Type *ty = underlying_type.codegen();
		//allocaV = create_entry_block_alloca(builder->GetInsertBlock()->getParent(), name, ty);
	}
	return allocaV;
}

llvm::Value* AST::VariableDef::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name) {
	auto allocaV = var_ref->codegen(builder);
	
	//if (builder) {
		//emitLocation(builder, this);
		//auto *alloca = create_entry_block_alloca(builder->GetInsertBlock()->getParent(), name, ty);
		builder->CreateLifetimeStart(allocaV);
		if (value) {
			auto v = value->codegen(builder);
			builder->CreateStore(v, allocaV);
		}
		//auto alloca = builder->CreateAlloca(getCodegenType(type.get()), nullptr, name);
		//NamedValues.front()[name] = std::pair<llvm::Type*, llvm::Value*>(ty, alloca); // Local variable

		return builder->CreateLoad(allocaV);
	/*} else {
		TheModule->getOrInsertGlobal(name, ty);
		auto global = TheModule->getGlobalVariable(name);

		NumericLiteral *init_val = dynamic_cast<NumericLiteral*>(value.get());
		if (!init_val) throw std::runtime_error("Can only initialise global with literal");
		llvm::ConstantInt *v = static_cast<llvm::ConstantInt*>(init_val->codegen(builder));

		global->setInitializer(v);
		//NamedValues.front()[name] = std::pair<llvm::Type*, llvm::Value*>(ty, global); // Global variable
		return nullptr;
	}*/
}

/*llvm::Value* AST::VariableRef::getFieldAlloca(llvm::IRBuilder<> *builder) {
	std::vector<llvm::Value*> fieldIdx;

	auto t = varRef->getVarType();
	auto i32 = AST::get_fundamental_type("i32").codegen();

	auto idxs = t.get_field_idxs(fields);

	auto currentT = t;
	fieldIdx.push_back(llvm::ConstantInt::get(i32, llvm::APInt(32, 0)));

	for (auto &idx : idxs) {
		fieldIdx.push_back(llvm::ConstantInt::get(i32, llvm::APInt(32, idx)));
	}

	return builder->CreateGEP(t.codegen(), varRef->getAlloca(), llvm::ArrayRef<llvm::Value*>(fieldIdx));
}

llvm::Value* AST::VariableRef::codegen(llvm::IRBuilder<> *builder, std::string name) {
	//emitLocation(builder, this);
	auto t = varRef->getVarType();
	std::string type_str_src;
	llvm::raw_string_ostream rso(type_str_src);
	t.codegen()->print(rso);

	return builder->CreateLoad(getFieldAlloca(builder), name);
}*/

// VARIABLE DEFINITION
// variable_def = 'let' identifier (':' type)? ('=' binary_expr)?;
std::unique_ptr<AST::Expression> Parser::parse_variable_def(FILE* f) {
	EXPECT(tok_let, "for variable definition");
	//get_next_token(f); // Trim the let
	//if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "identifier after 'let'");
	std::string name = EXPECT_IDENTIFIER("variable name");//std::get<std::string>(currentTokenVal);
	//get_next_token(f); // Trim the identifier

	std::optional<AST::TypeInstance> type;

	OPTIONAL(':', {
		type = parse_type(f);
		if (currentToken != '=') return std::make_unique<AST::VariableDef>(name, std::move(*type), nullptr);
	});

	EXPECT('=', " or type after variable name");

	/*if (currentToken == ':') { //throw UNEXPECTED_CHAR(currentToken, "expected ':' after variable name");
		get_next_token(f); // Trim the :
		type = parse_type(f);

		if (currentToken != '=') return std::make_unique<AST::VariableDef>(name, std::move(*type), nullptr);
	}*/

	//if (currentToken != '=') throw UNEXPECTED_CHAR(currentToken, "expected `=` or type after variable name");
	//get_next_token(f); // Trim the =

	std::unique_ptr<AST::Expression> val = parse_pratt_expression(f);
	if (!type) type = val->getType();
	return std::make_unique<AST::VariableDef>(name, std::move(*type), std::move(val));
}

std::shared_ptr<AST::MemoryLoc> AST::get_var(std::string var) {
	for (auto &scope_vars: stack_allocations) {
		if (scope_vars.find(var) != scope_vars.end()) {
			auto m = scope_vars[var];
			auto v = std::dynamic_pointer_cast<AST::Variable>(m);
			if (!v) throw std::runtime_error("ummmmm... what?");
			return v;
		}
	}

	auto gv = GlobalNamespace->get_var(var);
	if (gv) return gv;

	//std::cerr << fmt::format("Couldn't find variable {} in current scope", var) << std::endl;
	return nullptr;
}

/*std::shared_ptr<AST::Variable> AST::get_var(std::string var) {
	for (auto &scopeVars: NamedValues) {
		if (scopeVars.find(var) != scopeVars.end()) return scopeVars[var];
	}

	throw std::runtime_error(fmt::format("Couldn't find variable {} in current scope", var));
}*/

std::shared_ptr<AST::Variable> AST::Variable::create_in_scope(std::string name, AST::TypeInstance type) {
	if (AST::get_var(name)) std::cerr << "WARNING: Redeclaration of variable " << name << std::endl;
	auto v = std::make_shared<AST::Variable>(name, type);
	stack_allocations.front()[name] = v;
	return v;
}

/*std::shared_ptr<AST::Variable> AST::Variable::create(std::string name, AST::TypeInstance type) {
	auto v = std::make_shared<AST::Variable>(name, type);
	NamedValues.front()[name] = v;
	return v;
}*/
