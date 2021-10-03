/*#include "casting.hpp"

#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/IRBuilder.h>
#include <fmt/format.h>

llvm::Value* create_cast(llvm::IRBuilder<> *builder, llvm::Value *src, llvm::Type *dest) {
	auto s = src->getType();
	if (s->isIntegerTy() && dest->isIntegerTy()) {
		return builder->CreateIntCast(src, dest, false);
	} else if (s->isFloatingPointTy() && dest->isFloatingPointTy()) {
		return builder->CreateFPCast(src, dest);
	} else if (s->isIntegerTy() && dest->isFloatingPointTy()) {
		return builder->CreateUIToFP(src, dest);
	} else if (s->isFloatingPointTy() && dest->isIntegerTy()) {
		return builder->CreateFPToUI(src, dest);
	}
	std::string type_str_src;
	llvm::raw_string_ostream rso(type_str_src);
	s->print(rso);
	std::string type_str_dest;
	llvm::raw_string_ostream rso2(type_str_dest);
	dest->print(rso2);
	throw std::runtime_error(fmt::format("Could not create cast from {} to {}", type_str_src, type_str_dest));
}*/