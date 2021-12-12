#ifndef _GEP_H
#define _GEP_H

#include "memory_location.hpp"

namespace AST {
	class GEP: public AST::MemoryLoc {
		private:
		AST::Expression* to_index;
		int idx;
		
		public:
		GEP(AST::Expression* to_index, int idx): MemoryLoc(std::get<AST::FieldInfoField>(to_index->getType().get_field_info_by_index(idx)).type, ""), to_index(std::move(to_index)), idx(idx) {}
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
	};
}

#endif
