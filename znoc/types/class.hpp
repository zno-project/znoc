#ifndef _ZNO_CLASS_H
#define _ZNO_CLASS_H

#include "type.hpp"
#include <llvm/IR/Type.h>
#include "../constructions/function.hpp"

namespace AST {
	class ClassType: public Type {
		llvm::StructType *generated;
		std::vector<std::unique_ptr<AST::Function>> functions;
		
		public:
		ClassType(std::string name, std::vector<AST::field_type_t> fieldTypes, std::map<std::string, int> fieldIndices, std::vector<std::unique_ptr<AST::Function>> functions) : Type(name, std::move(fieldTypes), std::move(fieldIndices), std::vector<std::vector<AST::type_usage_t>>()), generated(nullptr), functions(std::move(functions)) {}
		virtual llvm::Type* codegen(int template_instance);

	};
}

namespace Parser {
	std::shared_ptr<AST::Type>  parse_class_def(FILE* f);
}

#endif