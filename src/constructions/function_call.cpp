#include "function_call.hpp"

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
#include "../types/casting.hpp"
#include "reference.hpp"
#include "../macros.hpp"
#include "../parsing.hpp"
#include "codeblock.hpp"
#include "../mangling.hpp"
#include "../types/builtins.hpp"

llvm::Value* AST::FunctionCall::codegen(llvm::IRBuilder<> *builder, std::string name) {
	/*llvm::Function *F = function->generated;

	std::vector<llvm::Value*> fargs = std::vector<llvm::Value*>();

	for (auto &arg: args) {
		auto aV = arg->codegen(builder, "");
		fargs.push_back(aV);
	}

	auto funcCall = builder->CreateCall(F, llvm::ArrayRef<llvm::Value*>(fargs), name);
	return funcCall;*/
	return nullptr;
}
