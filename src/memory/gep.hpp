#ifndef _GEP_H
#define _GEP_H

#include "memory_location.hpp"

namespace AST {
	class GEP: public AST::MemoryLoc {
		private:
		int idx;
		AST::Expression* to_index;
		
		public:
		GEP(AST::Expression* to_index, int idx): idx(idx), to_index(std::move(to_index)), MemoryLoc(std::get<AST::FieldInfoField>(to_index->getType().get_field_info_by_index(idx)).type) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder);
	};
}

#endif
