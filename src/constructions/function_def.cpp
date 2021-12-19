#include "function_def.hpp"

#include "../types/type.hpp"
#include "../memory/variable.hpp"
#include "../memory/memory_location.hpp"
#include "../attributes.hpp"
#include "../llvm_module.hpp"
#include "../parsing.hpp"
#include "../macros.hpp"
#include "construction_parse.hpp"
#include "../main.hpp"

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
#include "../types/type_base.hpp"

llvm::Value* AST::Function::codegen(__attribute__((unused)) llvm::IRBuilder<> *builder) {
	if (allocaV) return allocaV;

	codegen_prototype();
	llvm::Function *F = static_cast<llvm::Function*>(allocaV);

	if (body) {
		llvm::BasicBlock *block = llvm::BasicBlock::Create(*TheContext, "entry", F);
		llvm::IRBuilder<> builder(block);

		auto Fargs = F->arg_begin();

		for (unsigned long i = 0; i < args.size(); i++) {
			llvm::Value *a = Fargs++;
			a->setName(args[i]->getName());

			auto a_stack = args[i]->codegen(&builder);
			builder.CreateStore(a, a_stack);
		}

		llvm::Value *va_list_alloca;
		if (varargs_name.has_value()) {
			auto vastart = llvm::Intrinsic::getDeclaration(&*TheModule, llvm::Intrinsic::vastart);
			
			va_list_alloca = varargs_var->codegen(&builder);

			auto va_list_ptr = builder.CreateBitCast(va_list_alloca, llvm::Type::getInt8PtrTy(*TheContext));
			auto va_list_arg = builder.CreateCall(vastart, {va_list_ptr});
			auto va_iter_init_func = GlobalNamespace->get_namespace_by_name("std")->get_namespace_by_name("variadic")->get_namespace_by_name("VariadicIterator")->get_var("init");
			auto va_list_len = builder.CreateLoad(args[args.size()-1]->codegen(&builder));
			builder.CreateCall(static_cast<llvm::Function*>(va_iter_init_func->codegen(&builder)), {va_list_alloca, va_list_len});
		}

		body->codegen(&builder);

		if (varargs_name.has_value()) {
			auto vaend = llvm::Intrinsic::getDeclaration(&*TheModule, llvm::Intrinsic::vaend);
			auto valist_ty = GlobalNamespace->get_namespace_by_name("std")->get_namespace_by_name("variadic")->get_type_by_name("VariadicIterator").codegen();
			auto va_list_ptr = builder.CreateBitCast(va_list_alloca, llvm::Type::getInt8PtrTy(*TheContext));
			auto va_list_arg = builder.CreateCall(vaend, {va_list_ptr});
		}

		if (!builder.GetInsertBlock()->getTerminator()) builder.CreateRetVoid(); // If no terminator, add a `ret void`

		verifyFunction(*F, &llvm::errs());
		//TheFPM->run(*F);
	}

	return allocaV = (llvm::Value*)F;
}

void AST::Function::codegen_prototype() {
	std::vector<llvm::Type*> fargs = std::vector<llvm::Type*>();

	for (auto &arg: args) {
		fargs.push_back(arg->underlying_type.codegen());
	}

	llvm::FunctionType *ft = llvm::FunctionType::get(returnType.codegen(), fargs, varargs_name.has_value());
	llvm::FunctionCallee f = TheModule->getOrInsertFunction(this->get_name(), ft);

	auto f2 = (llvm::Function*)(f.getCallee());
	f2->setCallingConv(llvm::CallingConv::C);

	if (attributes[Attributes::AlwaysInline]) f2->addFnAttr(llvm::Attribute::AlwaysInline);
	if (attributes[Attributes::Extern]) f2->addFnAttr("extern");
	if (is_member_func) f2->addFnAttr("member_func");
	allocaV = f2;
}

// FUNCTION
// function = function_prototype (codeblock | ';');
std::shared_ptr<AST::Function> Parser::parse_function(zno_ifile& f, attributes_t attributes, std::optional<AST::TypeInstance> self_type) {
	push_new_scope(); // Create new scope

	EXPECT(tok_func, "to start function definition");
	std::string name = EXPECT_IDENTIFIER("function name after 'func'");

	typedef std::pair<std::string, AST::TypeInstance> arg_t;
	std::vector<arg_t> argsP;
	bool is_member_func = false;
	std::optional<std::string> varargs_name;

	LIST('(', ',', ')', {
		std::string name = EXPECT_IDENTIFIER("argument name");

		if (argsP.size() == 0 && name == "self" && self_type.has_value()) {
			auto arg = arg_t("self", self_type.value().get_pointer_to());
			argsP.push_back(std::move(arg));
			is_member_func = true;
		} else {
			EXPECT(':', "after argument name");
			if (currentToken == '.') {
				EXPECT('.', "variadic");
				EXPECT('.', "variadic");
				EXPECT('.', "variadic");
				varargs_name = name;
				if (!attributes[Attributes::Extern]) argsP.push_back(arg_t(":zno_va_arg_count", AST::get_fundamental_type("i32")));
			} else {
				AST::TypeInstance type = parse_type(f);
				auto arg = arg_t(name, std::move(type));
				argsP.push_back(std::move(arg));
			}
		}
	}, "argument list");

	std::vector<std::shared_ptr<AST::Variable>> args;
	for (auto &a : argsP) {
		args.push_back(AST::Variable::create_in_scope(a.first, a.second));
	}

	std::shared_ptr<AST::Variable> varargs_var = nullptr;
	if (varargs_name.has_value()) {
		auto valist_ty = GlobalNamespace->get_namespace_by_name("std")->get_namespace_by_name("variadic")->get_type_by_name("VariadicIterator");
		varargs_var = AST::Variable::create_in_scope(varargs_name.value(), valist_ty);
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
	
	auto scope_pop = pop_scope();
	if (body) body->push_before_return(std::move(scope_pop));  // End scope

	return std::make_unique<AST::Function>(name, args, returnType, attributes, std::move(body), is_member_func, varargs_name, varargs_var);
}
