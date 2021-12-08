#ifndef _GEP_H
#define _GEP_H

#include "memory_location.hpp"

namespace AST {
	class GEP: public AST::MemoryLoc {
		private:
		int idx;
		std::shared_ptr<MemoryLoc> to_index = nullptr;
		AST::Expression* to_index_expr = nullptr;
		
		public:
		GEP(std::shared_ptr<MemoryLoc> to_index, int idx): idx(idx), to_index(std::move(to_index)), MemoryLoc(std::get<AST::FieldInfoField>(to_index->underlying_type.get_field_info_by_index(idx)).type) {}
		GEP(AST::Expression* to_index, int idx): idx(idx), to_index_expr(std::move(to_index)), MemoryLoc(std::get<AST::FieldInfoField>(to_index->getType().get_field_info_by_index(idx)).type) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder);
	};
}

#endif
