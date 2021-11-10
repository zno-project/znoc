#include "casting.hpp"
#include <fmt/format.h>
#include <llvm/Support/raw_ostream.h>

std::unique_ptr<AST::Cast> AST::make_cast(std::unique_ptr<AST::Expression> from, AST::TypeInstance to) {
	return std::make_unique<AST::Cast>(std::move(from), std::move(to));
}

llvm::Value* create_cast(llvm::IRBuilder<> *builder, llvm::Value *src, llvm::Type *dest, std::string name) {
	auto s = src->getType();
	if (s->isIntegerTy() && dest->isIntegerTy()) {
		return builder->CreateIntCast(src, dest, false, name);
	} else if (s->isFloatingPointTy() && dest->isFloatingPointTy()) {
		return builder->CreateFPCast(src, dest, name);
	} else if (s->isIntegerTy() && dest->isFloatingPointTy()) {
		return builder->CreateUIToFP(src, dest, name);
	} else if (s->isFloatingPointTy() && dest->isIntegerTy()) {
		return builder->CreateFPToUI(src, dest, name);
	}
	std::string type_str_src;
	llvm::raw_string_ostream rso(type_str_src);
	s->print(rso);
	std::string type_str_dest;
	llvm::raw_string_ostream rso2(type_str_dest);
	dest->print(rso2);
	throw std::runtime_error(fmt::format("Could not create cast from {} to {}", type_str_src, type_str_dest));
}

llvm::Value* AST::Cast::codegen(llvm::IRBuilder<> *builder, std::string name) {
	return create_cast(builder, from->codegen(builder), to.codegen(), name);
}
