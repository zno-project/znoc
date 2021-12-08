#include "gep.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constant.h>
#include "../llvm_module.hpp"

llvm::Value* AST::GEP::codegen(llvm::IRBuilder<> *builder) {
	auto zero = llvm::ConstantInt::get(*TheContext, llvm::APInt(32, 0));
	auto i = llvm::ConstantInt::get(*TheContext, llvm::APInt(32, idx));
	llvm::Value* a[] = {zero, i};
	auto idxs = llvm::ArrayRef<llvm::Value*>(a);

	llvm::Value *index_loc;
	if (to_index) index_loc = to_index->codegen(builder);
	else index_loc = to_index_expr->codegen_to_ptr(builder);

	allocaV = builder->CreateGEP(index_loc, idxs);
	return allocaV;
}
