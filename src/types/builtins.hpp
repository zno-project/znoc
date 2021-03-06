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
		llvm::Type* codegen(__attribute__((unused)) size_t template_instance) override {
			return llvm::IntegerType::get(*TheContext, N);
		}
	};

	class fundamental_half: public TypeBase {
		public:
		fundamental_half() : TypeBase("half") {}
		llvm::Type* codegen(__attribute__((unused)) size_t template_instance) override {
			return llvm::Type::getHalfTy(*TheContext);
		}
	};

	class fundamental_float: public TypeBase {
		public:
		fundamental_float() : TypeBase("float") {}
		llvm::Type* codegen(__attribute__((unused)) size_t template_instance) override {
			return llvm::Type::getFloatTy(*TheContext);
		}
	};

	class fundamental_double: public TypeBase {
		public:
		fundamental_double() : TypeBase("double") {}
		llvm::Type* codegen(__attribute__((unused)) size_t template_instance) override {
			return llvm::Type::getDoubleTy(*TheContext);
		}
	};

	class fundamental_fp128: public TypeBase {
		public:
		fundamental_fp128() : TypeBase("fp128") {}
		llvm::Type* codegen(__attribute__((unused)) size_t template_instance) override {
			return llvm::Type::getFP128Ty(*TheContext);
		}
	};

	class fundamental_void: public TypeBase {
		public:
		fundamental_void() : TypeBase("void") {}
		llvm::Type* codegen(__attribute__((unused)) size_t template_instance) override {
			return llvm::Type::getVoidTy(*TheContext);
		}
	};

	class fundamental_function: public TypeBase {
		public:
		fundamental_function() : TypeBase("function", {}, {}, {AST::Interface {}}) {}
		llvm::Type* codegen(size_t template_instance) override {
			auto this_instance = generic_types.at(template_instance);
			auto t = this_instance.at(0).codegen();
			
			std::vector<llvm::Type*> args;
			bool variadic = false;

			if (this_instance.size() > 1) {
				auto last_type = this_instance.at(this_instance.size() - 1);
				auto var_args_ty = AST::get_fundamental_type("var_args");
				if (typeid(*last_type.base_type) == typeid(*var_args_ty.base_type)) {
					variadic = true;
					this_instance.pop_back();
				}
			}

			for (auto arg = this_instance.begin() + 1; arg != this_instance.end(); arg++) {
				args.push_back(arg->codegen());
			}

			return llvm::FunctionType::get(t, llvm::ArrayRef<llvm::Type*>(args), variadic)->getPointerTo();
		}

		AST::TypeInstance get_return_of_function(size_t template_instance) {
			return generic_types.at(template_instance).at(0);
		}

		std::vector<AST::TypeInstance> get_args_of_function(size_t template_instance) {
			auto this_instance = generic_types.at(template_instance);
			return std::vector<AST::TypeInstance>(this_instance.begin() + 1, this_instance.end());
		}
	};

	class fundamental_function_varargs: public TypeBase {
		public:
		fundamental_function_varargs() : TypeBase("var_args") {}
		llvm::Type* codegen(__attribute__((unused)) size_t template_instance) override {
			throw std::runtime_error("var_args is not a valid type");
		}
	};

	class fundamental_namespace_ref: public TypeBase {
		public:
		fundamental_namespace_ref() : TypeBase("namespace_ref") {}
		llvm::Type* codegen(__attribute__((unused)) size_t template_instance) override {
			return llvm::FunctionType::get(llvm::Type::getVoidTy(*TheContext), false);
		}
	};

	class fundamental_ptr: public TypeBase {
		public:
		fundamental_ptr() : TypeBase("ptr", {}, {}, {AST::Interface {}}) {}
		llvm::Type* codegen(__attribute__((unused)) size_t template_instance) override {
			auto t = generic_types.at(template_instance).at(0).codegen();
			return t->getPointerTo();
		}

		AST::TypeInstance get_pointed_to(size_t template_instance) {
			return generic_types.at(template_instance).at(0);
		}
	};
}

#endif
