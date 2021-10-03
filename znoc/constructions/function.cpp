#include "function.hpp"

#include <string>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Verifier.h>
#include <fmt/format.h>

#include <map>
#include <vector>
#include <iostream>

#include "../attributes.hpp"
#include "../types/type.hpp"
#include "../main.hpp"
#include "../types/casting.hpp"
#include "reference.hpp"
#include "../macros.hpp"
#include "../parsing.hpp"
#include "codeblock.hpp"
#include "../mangling.hpp"

//std::map<std::string, AST::Function*> NamedFunctions; // Registered functions

llvm::AllocaInst* create_entry_block_alloca(llvm::Function *in_func, std::string name, llvm::Type* t) {
	llvm::IRBuilder<> tmpBuilder(&in_func->getEntryBlock(), in_func->getEntryBlock().begin());
	return tmpBuilder.CreateAlloca(t, nullptr, fmt::format("{}_stack", name));
}

llvm::Value* AST::FunctionCall::codegen(llvm::IRBuilder<> *builder, std::string name) {
	//emitLocation(builder, this);
	//auto function = AST::get_function_by_name(callee);

	llvm::Function *F = function->generated;

	std::vector<llvm::Value*> fargs = std::vector<llvm::Value*>();

	for (auto &arg: args) {
		auto aV = arg->codegen(builder, "");
		fargs.push_back(aV);
	}

	auto funcCall = builder->CreateCall(F, llvm::ArrayRef<llvm::Value*>(fargs), name);
	return funcCall;
}

llvm::Value* AST::Function::codegen() {
	llvm::Function *F = generated;

	if (body) {
		NamedValues.push_front(std::map<std::string, std::shared_ptr<AST::Variable>>()); // Create new scope

		llvm::BasicBlock *block = llvm::BasicBlock::Create(*TheContext, "entry", F);
		llvm::IRBuilder<> builder(block);

		auto Fargs = F->arg_begin();

		for (unsigned long i = 0; i < args.size(); i++) {
			llvm::Value *a = Fargs++;
			a->setName(args[i]->getName());

			auto a_stack = args[i]->codegen(&builder);
			builder.CreateStore(a, a_stack);
		}

		body->codegen(&builder, fmt::format("f_{}", name));

		if (!builder.GetInsertBlock()->getTerminator()) builder.CreateRetVoid(); // If no terminator, add a `ret void`

		verifyFunction(*F, &llvm::errs());
		//TheFPM->run(*F);

		NamedValues.pop_front(); // End scope
	}

	return nullptr;
}

/*std::string AST::Function::mangled_name() {
	if (attributes[(unsigned long)Attributes::NoMangle]) return name;
	/*if (name == "main") return name;
	/*auto p = manglePrefixes;
	p.push_back(std::pair<ManglePart, std::string>(ManglePart::Function, name));
	return full_mangle_name(p);
}*/

void AST::Function::codegen_prototype() {
	std::vector<llvm::Type*> fargs = std::vector<llvm::Type*>();

	for (auto &arg: args) {
		fargs.push_back(arg->getVarType().codegen());
	}

	llvm::FunctionType *ft = llvm::FunctionType::get(returnType.codegen(), fargs, false);
	llvm::FunctionCallee f = TheModule->getOrInsertFunction(this->get_name(), ft);

	generated = (llvm::Function*)(f.getCallee());
	generated->setCallingConv(llvm::CallingConv::C);
	if (attributes[(unsigned long)Attributes::AlwaysInline]) generated->addFnAttr(llvm::Attribute::AlwaysInline);

}

AST::type_usage_t AST::Function::getRetType() {
	return returnType;
}

// FUNCTION
// function = function_prototype (codeblock | ';');
std::unique_ptr<AST::Function> Parser::parse_function(FILE* f) {
	NamedValues.push_front(std::map<std::string, std::shared_ptr<AST::Variable>>()); // Create new scope

	// GET NAME
	get_next_token(f);
	if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "function name after 'func'");

	auto name = *std::get_if<std::string>(&currentTokenVal);
	get_next_token(f);

	// GET ARGS
	if (currentToken != '(') throw UNEXPECTED_CHAR(currentToken, "(");
	get_next_token(f);

	std::vector<std::pair<std::string, AST::type_usage_t>> argsP;
	if (currentToken != ')') while (1) {
		if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "argument name");

		// GET ARG NAME
		std::string name = *std::get_if<std::string>(&currentTokenVal);

		// GET TYPE NAME
		get_next_token(f);
		if (currentToken != ':') throw UNEXPECTED_CHAR(currentToken, ": after argument name");
		get_next_token(f);
		AST::type_usage_t type = parse_type(f);

		// CHECK IF END OF ARG LIST
		std::pair<std::string, AST::type_usage_t> arg = std::pair<std::string, AST::type_usage_t>(name, std::move(type));
		argsP.push_back(std::move(arg));
		if (currentToken == ')') break;
		else if (currentToken != ',') throw UNEXPECTED_CHAR(currentToken, ", or ) after argument");
		get_next_token(f);
	}
	get_next_token(f);

	std::vector<std::shared_ptr<AST::Variable>> args;
	for (auto &a : argsP) {
		args.push_back(AST::Variable::create(a.first, a.second));
	}

	// CHECK FOR RETURN TYPE
	AST::type_usage_t returnType;
	if (currentToken == '-') {
		get_next_token(f);
		
		if (currentToken != '>') throw UNEXPECTED_CHAR(currentToken, "->");
		get_next_token(f);
		returnType = parse_type(f);
	} else {
		returnType = AST::get_type_by_name("__llvm_void");
	}

	if (currentToken != '{') {
		get_next_token(f);
		auto r = std::make_unique<AST::Function>(name, args, returnType, currentAttributes, nullptr);
		NamedValues.pop_front(); // End scope
		return r;
	}

	std::unique_ptr<AST::CodeBlock> body = parse_code_block(f);

	NamedValues.pop_front(); // End scope

	auto r =  std::make_unique<AST::Function>(name, args, returnType, currentAttributes, std::move(body));
	return r;
}