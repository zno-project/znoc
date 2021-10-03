#ifndef _ZNO_BUILTINS_H
#define _ZNO_BUILTINS_H

#include "type.hpp"

#include <llvm/IR/Type.h>
#include "../main.hpp"
#include <fmt/format.h>

namespace AST {
	template <unsigned int N>
	class llvm_builtin_type_int: public Type {
		public:
		llvm_builtin_type_int() : Type(fmt::format("__llvm_i{}", N)) {}
		virtual llvm::Type* codegen(__attribute__((unused)) int template_instance) {
			return llvm::IntegerType::get(*TheContext, N);
		}
	};

	class llvm_builtin_type_half: public Type {
		public:
		llvm_builtin_type_half() : Type("__llvm_half") {}
		virtual llvm::Type* codegen(__attribute__((unused)) int template_instance) {
			return llvm::Type::getHalfTy(*TheContext);
		}
	};

	class llvm_builtin_type_float: public Type {
		public:
		llvm_builtin_type_float() : Type("__llvm_float") {}
		virtual llvm::Type* codegen(__attribute__((unused)) int template_instance) {
			return llvm::Type::getFloatTy(*TheContext);
		}
	};

	class llvm_builtin_type_double: public Type {
		public:
		llvm_builtin_type_double() : Type("__llvm_double") {}
		virtual llvm::Type* codegen(__attribute__((unused)) int template_instance) {
			return llvm::Type::getDoubleTy(*TheContext);
		}
	};

	class llvm_builtin_type_fp128: public Type {
		public:
		llvm_builtin_type_fp128() : Type("__llvm_fp128") {}
		virtual llvm::Type* codegen(__attribute__((unused)) int template_instance) {
			return llvm::Type::getFP128Ty(*TheContext);
		}
	};

	class llvm_builtin_type_void: public Type {
		public:
		llvm_builtin_type_void() : Type("__llvm_void") {}
		virtual llvm::Type* codegen(__attribute__((unused)) int template_instance) {
			return llvm::Type::getVoidTy(*TheContext);
		}
	};

	class llvm_builtin_type_ptr: public Type {
		public:
		llvm_builtin_type_ptr() : Type("__llvm_ptr") {}
		virtual llvm::Type* codegen(__attribute__((unused)) int template_instance) {
			return llvm::Type::getInt32PtrTy(*TheContext);
		}
	};
}

#endif