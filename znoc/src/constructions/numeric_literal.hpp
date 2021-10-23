#ifndef _NUMERICAL_LITERAL_H
#define _NUMERICAL_LITERAL_H

#include "expression.hpp"
#include "../types/builtins.hpp"
#include <llvm/IR/Constant.h>
#include <llvm/ADT/APFloat.h>
#include <cfloat>

namespace AST {
	class NumericLiteral: public AST::Expression {
		union {
			uint64_t i;
			double f;
		} value;
		bool is_int_val;

		static bool is_int(double v) {
			uint64_t vi = v;
			double vc = v - vi;
			return vc == 0;
		}


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

		NumericLiteral(double v): Expression(is_int(v) ? (v > UINT32_MAX ? AST::get_fundamental_type("i64") : AST::get_fundamental_type("i32")) : (v > FLT_MAX ? AST::get_fundamental_type("double") : AST::get_fundamental_type("float"))) {
			is_int_val = is_int(v);
			if (is_int_val) value.i = v;
			else value.f = v;
		}

		static std::unique_ptr<NumericLiteral> NewInt(uint64_t v, size_t len) {
			return std::make_unique<AST::NumericLiteral>((uint64_t)v, len);
		}

		static std::unique_ptr<NumericLiteral> NewFloat(double v, size_t len) {
			return std::make_unique<AST::NumericLiteral>((double)v, len);
		}
		virtual llvm::Value* codegen(__attribute__((unused)) llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name = "");
	};
}

#endif