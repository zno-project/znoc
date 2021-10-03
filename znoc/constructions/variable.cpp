#include "variable.hpp"
#include "expression.hpp"
#include "../types/type.hpp"
#include "binary_op.hpp"
#include "reference.hpp"
#include "function.hpp"
#include "../casting.hpp"
#include "../macros.hpp"
#include "../parsing.hpp"
#include "numeric_literal.hpp"

#include <llvm/IR/Module.h>
#include <llvm/IR/Constant.h>
#include <iostream>

std::deque<std::map<std::string, std::shared_ptr<AST::Variable>>> NamedValues; // Deque of scopes, containing a map of variables in scope

llvm::Value* AST::Variable::codegen(llvm::IRBuilder<> *builder) {
	llvm::Type *ty = type.codegen();
	allocaV = create_entry_block_alloca(builder->GetInsertBlock()->getParent(), name, ty);
	return allocaV;
}

llvm::Value* AST::VariableDef::codegen(llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name) {
	auto allocaV = varRef->codegen(builder);
	
	//if (builder) {
		//emitLocation(builder, this);
		//auto *alloca = create_entry_block_alloca(builder->GetInsertBlock()->getParent(), name, ty);
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

llvm::Value* AST::VariableRef::getFieldAlloca(llvm::IRBuilder<> *builder) {
	std::vector<llvm::Value*> fieldIdx;

	auto t = varRef->getVarType();
	auto i32 = AST::get_type_by_name("__llvm_i32").codegen();

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

	std::cout << "variable ref to " << varRef->getName() << " of type " << typeid(*varRef->getVarType().type).name() << "/" << type_str_src << std::endl;
	
	return builder->CreateLoad(getFieldAlloca(builder), name);
}

// VARIABLE DEFINITION
// variable_def = 'let' identifier (':' type)? ('=' binary_expr)?;
std::unique_ptr<AST::Expression> Parser::parse_variable_def(FILE* f) {
	get_next_token(f); // Trim the let
	if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "identifier after 'let'");
	std::string name = *std::get_if<std::string>(&currentTokenVal);
	get_next_token(f); // Trim the identifier

	AST::type_usage_t type;

	if (currentToken == ':') { //throw UNEXPECTED_CHAR(currentToken, "expected ':' after variable name");
		get_next_token(f); // Trim the :
		type = parse_type(f);

		if (currentToken != '=') return std::make_unique<AST::VariableDef>(name, std::move(type), nullptr);
	}

	if (currentToken != '=') throw UNEXPECTED_CHAR(currentToken, "expected `=` or type after variable name");
	get_next_token(f); // Trim the =

	std::unique_ptr<AST::Expression> val = parse_binary_expression(f);
	if (!type) type = val->getType();
	return std::make_unique<AST::VariableDef>(name, std::move(type), std::move(val));
}

std::shared_ptr<AST::Variable> AST::get_var(std::string var) {
	for (auto &scopeVars: NamedValues) {
		if (scopeVars.find(var) != scopeVars.end()) return scopeVars[var];
	}

	throw std::runtime_error(fmt::format("Couldn't find variable {} in current scope", var));
}

std::shared_ptr<AST::Variable> AST::Variable::create(std::string name, AST::type_usage_t type) {
	auto v = std::make_shared<AST::Variable>(name, type);
	NamedValues.front()[name] = v;
	return v;
}