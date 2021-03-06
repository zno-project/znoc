#ifndef _MEMORY_REF_H
#define _MEMORY_REF_H

#include "../constructions/expression.hpp"
#include "memory_location.hpp"

namespace AST {
	class MemoryRef: public Expression {
		private:
		std::shared_ptr<MemoryLoc> to_access;
		public:
		MemoryRef(std::shared_ptr<MemoryLoc> to_access): Expression(to_access->underlying_type), to_access(to_access) {}
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override {
			return builder->CreateLoad(to_access->codegen(builder));
		}
		llvm::Value* codegen_to_underlying_ptr(llvm::IRBuilder<> *builder) {
			return to_access->codegen(builder);
		}
	};
}

#endif
