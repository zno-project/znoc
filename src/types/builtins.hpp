#ifndef _ZNO_BUILTINS_H
#define _ZNO_BUILTINS_H

#include "type.hpp"

#include <llvm/IR/Type.h>
#include "../llvm_module.hpp"
#include <fmt/format.h>
#include "type_base.hpp"

namespace AST {
	template <unsigned int N>
	class fundamental_int: public TypeBase {
		public:
		fundamental_int() : TypeBase(fmt::format("i{}", N)) {}
		virtual llvm::Type* codegen(__attribute__((unused)) size_t template_instance) {
			return llvm::IntegerType::get(*TheContext, N);
		}
	};

	class fundamental_half: public TypeBase {
		public:
		fundamental_half() : TypeBase("half") {}
		virtual llvm::Type* codegen(__attribute__((unused)) size_t template_instance) {
			return llvm::Type::getHalfTy(*TheContext);
		}
	};

	class fundamental_float: public TypeBase {
		public:
		fundamental_float() : TypeBase("float") {}
		virtual llvm::Type* codegen(__attribute__((unused)) size_t template_instance) {
			return llvm::Type::getFloatTy(*TheContext);
		}
	};

	class fundamental_double: public TypeBase {
		public:
		fundamental_double() : TypeBase("double") {}
		virtual llvm::Type* codegen(__attribute__((unused)) size_t template_instance) {
			return llvm::Type::getDoubleTy(*TheContext);
		}
	};

	class fundamental_fp128: public TypeBase {
		public:
		fundamental_fp128() : TypeBase("fp128") {}
		virtual llvm::Type* codegen(__attribute__((unused)) size_t template_instance) {
			return llvm::Type::getFP128Ty(*TheContext);
		}
	};

	class fundamental_void: public TypeBase {
		public:
		fundamental_void() : TypeBase("void") {}
		virtual llvm::Type* codegen(__attribute__((unused)) size_t template_instance) {
			return llvm::Type::getVoidTy(*TheContext);
		}
	};

	class fundamental_function: public TypeBase {
		public:
		fundamental_function() : TypeBase("function", {}, {}, {AST::Interface {}}) {}
		virtual llvm::Type* codegen(__attribute__((unused)) size_t template_instance) {
			auto t = generic_types.at(template_instance).at(0).codegen();
			return llvm::FunctionType::get(t, false)->getPointerTo();
		}

		AST::TypeInstance get_return_of_function(size_t template_instance) {
			return generic_types.at(template_instance).at(0);
		}
	};

	class fundamental_namespace_ref: public TypeBase {
		public:
		fundamental_namespace_ref() : TypeBase("namespace_ref") {}
		virtual llvm::Type* codegen(__attribute__((unused)) size_t template_instance) {
			return llvm::FunctionType::get(llvm::Type::getVoidTy(*TheContext), false);
		}
	};

	class fundamental_ptr: public TypeBase {
		public:
		fundamental_ptr() : TypeBase("ptr", {}, {}, {AST::Interface {}}) {}
		virtual llvm::Type* codegen(__attribute__((unused)) size_t template_instance) {
			auto t = generic_types.at(template_instance).at(0).codegen();
			return t->getPointerTo();
		}

		AST::TypeInstance get_pointed_to(size_t template_instance) {
			return generic_types.at(template_instance).at(0);
		}
	};
}

#endif
