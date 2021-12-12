#ifndef _NUMERICAL_LITERAL_H
#define _NUMERICAL_LITERAL_H

#include "expression.hpp"
#include <llvm/IR/Constant.h>
#include <llvm/ADT/APFloat.h>
#include <cfloat>
#include "../llvm_module.hpp"

namespace AST {
	class NumericLiteral: public AST::Expression {
		private:
		union {
			uint64_t i;
			double f;
		} value;
		bool is_int_val;

		static AST::TypeInstance len_to_ftype(size_t len) {
			switch (len) {
				case 16:
					return AST::get_fundamental_type("half");
				case 32:
					return AST::get_fundamental_type("float");
				case 64:
					return AST::get_fundamental_type("double");
				case 128:
					return AST::get_fundamental_type("fp128");
				default:
					throw std::runtime_error(fmt::format("Cannot get float with length {}", len));
			}
		}

		static AST::TypeInstance len_to_itype(size_t len) {
			switch (len) {
				case 1:
					return AST::get_fundamental_type("i1");
				case 8:
					return AST::get_fundamental_type("i8");
				case 16:
					return AST::get_fundamental_type("i16");
				case 32:
					return AST::get_fundamental_type("i32");
				case 64:
					return AST::get_fundamental_type("i64");
				case 128:
					return AST::get_fundamental_type("i128");
				default:
					throw std::runtime_error(fmt::format("Cannot get int with length {}", len));
			}
		}

		public:
		NumericLiteral(double v, size_t len): Expression(len_to_ftype(len)) {
			is_int_val = false;
			value.f = v;
		}

		NumericLiteral(uint64_t v, size_t len): Expression(len_to_itype(len)) {
			is_int_val = true;
			value.i = v;
		}

		NumericLiteral(double v, bool contains_dp): Expression(
			contains_dp ?
				AST::get_fundamental_type("double") :
				(v > INT32_MAX ?
					AST::get_fundamental_type("i64") :
					AST::get_fundamental_type("i32")
				)
		) {
			is_int_val = !contains_dp;
			if (is_int_val) value.i = v;
			else value.f = v;
		}

		static std::unique_ptr<NumericLiteral> NewUInt(uint64_t v, size_t len) {
			return std::make_unique<AST::NumericLiteral>((uint64_t)v, len);
		}

		static std::unique_ptr<NumericLiteral> NewInt(uint64_t v, size_t len) {
			return std::make_unique<AST::NumericLiteral>((uint64_t)v, len);
		}

		static std::unique_ptr<NumericLiteral> NewFloat(double v, size_t len) {
			return std::make_unique<AST::NumericLiteral>((double)v, len);
		}

		llvm::Value* codegen(__attribute__((unused)) llvm::IRBuilder<> *builder) override;

		llvm::Constant* codegen_const() override {
			if (is_int_val) {
				return llvm::ConstantInt::get(expressionType.codegen(), value.i);
			}
			return llvm::ConstantFP::get(*TheContext, llvm::APFloat(value.f));
		};

		std::string print() const override {
			if (is_int_val) {
				return std::to_string(value.i);
			}
			return std::to_string(value.f);
		}
	};
}

#endif
