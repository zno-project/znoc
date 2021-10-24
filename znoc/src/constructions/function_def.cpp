#include "function_def.hpp"

#include "../types/type.hpp"
#include "../types/builtins.hpp"
#include "../memory/variable.hpp"
#include "../attributes.hpp"
#include "../llvm_module.hpp"
#include "../parsing.hpp"
#include "../macros.hpp"
#include "construction_parse.hpp"

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>
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

llvm::Value* AST::Function::codegen() {
	llvm::Function *F = generated;

	if (body) {
		stack_allocations.push_front(std::map<std::string, std::shared_ptr<AST::MemoryLoc>>()); // Create new scope

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

		stack_allocations.pop_front(); // End scope
	}

	return nullptr;
}

void AST::Function::codegen_prototype() {
	std::vector<llvm::Type*> fargs = std::vector<llvm::Type*>();

	for (auto &arg: args) {
		fargs.push_back(arg->underlying_type.codegen());
	}

	llvm::FunctionType *ft = llvm::FunctionType::get(returnType.codegen(), fargs, false);
	llvm::FunctionCallee f = TheModule->getOrInsertFunction(this->get_name(), ft);

	generated = (llvm::Function*)(f.getCallee());
	generated->setCallingConv(llvm::CallingConv::C);

	//std::cout << attributes << std::endl;

	if (attributes[(unsigned long)Attributes::AlwaysInline]) generated->addFnAttr(llvm::Attribute::AlwaysInline);
}

// FUNCTION
// function = function_prototype (codeblock | ';');
std::shared_ptr<AST::Function> Parser::parse_function(FILE* f, std::optional<AST::TypeInstance> self_type) {
	stack_allocations.push_front(std::map<std::string, std::shared_ptr<AST::MemoryLoc>>()); // Create new scope

	EXPECT(tok_func, "to start function definition");
	auto name = EXPECT_IDENTIFIER("function name after 'func'");

	typedef std::pair<std::string, AST::TypeInstance> arg_t;
	std::vector<arg_t> argsP;

	if (self_type.has_value()) {
		auto arg = arg_t("self", std::move(self_type.value().get_pointer_to()));
		argsP.push_back(std::move(arg));
	}

	LIST('(', ',', ')', {
		std::string name = EXPECT_IDENTIFIER("argument name");
		EXPECT(':', "after argument name");
		AST::TypeInstance type = parse_type(f);
		auto arg = arg_t(name, std::move(type));
		argsP.push_back(std::move(arg));
	}, "argument list");

	std::vector<std::shared_ptr<AST::Variable>> args;
	for (auto &a : argsP) {
		args.push_back(AST::Variable::create_in_scope(a.first, a.second));
	}

	// CHECK FOR RETURN TYPE
	AST::TypeInstance returnType;

	IF_TOK_ELSE('-', {
		EXPECT('>', "in func return type");
		returnType = parse_type(f);
	}, {
		returnType = AST::get_fundamental_type("void");
	});

	std::unique_ptr<AST::CodeBlock> body = nullptr;

	IF_TOK_ELSE(';', {
		// Just trim it
	}, {
		body = std::unique_ptr<AST::CodeBlock>(static_cast<AST::CodeBlock*>(parse_code_block(f).release()));
	});

	stack_allocations.pop_front(); // End scope

	return std::make_unique<AST::Function>(name, args, returnType, currentAttributes, std::move(body));
}