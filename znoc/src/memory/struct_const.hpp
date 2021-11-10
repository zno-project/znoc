#ifndef _ZNO_STRUCT_CONST_H
#define _ZNO_STRUCT_CONST_H

#include "memory_location.hpp"
#include "../llvm_module.hpp"
#include <iostream>

namespace AST {
	class StructConstAccess: public AST::MemoryLoc {
		private:
		std::string linkage_name;

		public:
		StructConstAccess(std::string linkage_name, AST::TypeInstance ty): linkage_name(std::move(linkage_name)), MemoryLoc(ty) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder) {
			auto global = TheModule->getGlobalVariable(linkage_name, true);
			return global;
		}
	};
}

#endif
