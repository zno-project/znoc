#ifndef _STRUCT_TYPE_H
#define _STRUCT_TYPE_H

#include "type.hpp"
#include <llvm/IR/Type.h>

namespace AST {
	class StructType: public Type {
		std::map<int, llvm::StructType*> generated;

		public:
		StructType(std::string name, std::vector<AST::field_type_t> fieldTypes, std::map<std::string, int> fieldIndices, std::vector<std::vector<AST::type_usage_t>> templatedTypes) : Type(name, std::move(fieldTypes), std::move(fieldIndices), std::move(templatedTypes)), generated() {}
		virtual llvm::Type* codegen(int fieldsIndex);
	};
}

namespace Parser {
	std::shared_ptr<AST::Type> parse_struct_def(FILE* f);
}

#endif