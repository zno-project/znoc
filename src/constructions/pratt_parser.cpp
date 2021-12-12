#include "pratt_parser.hpp"
#include "expression.hpp"
#include "../parsing.hpp"
#include "construction_parse.hpp"
#include <memory>
#include <stdio.h>
#include "../memory/memory_ref.hpp"
#include "reference.hpp"

const char *operator_to_string[] = {".", "::", "+", "-", "*", "/", "%", "&", "|", "~", "^", "&&", "||", "!", "<<", ">>", "==", "!=", "<", ">", "<=", ">=", "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=", "is", "["};

void advance_op(operators op, FILE *f) {
	switch (op) {
		case dot:
		case plus:
		case minus:
		case star:
		case divide:
		case mod:
		case ampersand:
		case bitwise_or:
		case bitwise_not:
		case bitwise_xor:
		case logical_not:
		case assign:
		case subscript:
		case compare_lt:
		case compare_gt:
		case call:
			get_next_token(f);
			break;
		case double_colon:
		case logical_and:
		case logical_or:
		case shift_left:
		case shift_right:
		case compare_eq:
		case compare_neq:
		case compare_lte:
		case compare_gte:
		case assign_add:
		case assign_sub:
		case assign_mul:
		case assign_div:
		case assign_mod:
		case assign_bitwise_and:
		case assign_bitwise_or:
		case assign_bitwise_xor:
			get_next_token(f);
			get_next_token(f);
			break;
		case assign_shift_left:
		case assign_shift_right:
			get_next_token(f);
			get_next_token(f);
			get_next_token(f);
			break;
	}
}

operators parse_operator(FILE *f) {
	switch (currentToken) {
		case '.': {
			return dot;
		}
		case ':': {
			if (peek_next_token(f) != ':') throw std::runtime_error("Expected '::'");
			return double_colon;
		}
		case '+': {
			if (peek_next_token(f) == '=') {
				return assign_add;
			}
			return plus;
		}
		case '-': {
			if (peek_next_token(f) == '=') {
				return assign_sub;
			}
			return minus;
		}
		case '*': {
			if (peek_next_token(f) == '=') {
				return assign_mul;
			}
			return star;
		}
		case '/': {
			if (peek_next_token(f) == '=') {
				return assign_div;
			}
			return divide;
		}
		case '%': {
			if (peek_next_token(f) == '=') {
				return assign_mod;
			}
			return mod;
		}
		case '&': {
			if (peek_next_token(f) == '&') {
				return logical_and;
			} else if (peek_next_token(f) == '=') {
				return assign_bitwise_and;
			}
			return ampersand;
		}
		case '|': {
			if (peek_next_token(f) == '|') {
				return logical_or;
			} else if (peek_next_token(f) == '=') {
				return assign_bitwise_or;
			}
			return bitwise_or;
		}
		case '~': {
			return bitwise_not;
		}
		case '^': {
			if (peek_next_token(f) == '=') {
				return assign_bitwise_xor;
			}
			return bitwise_xor;
		}
		case '<': {
			if (peek_next_token(f) == '<') {
				if (peek_next_token(f, 2) == '=') {
					return assign_shift_left;
				}
				return shift_left;
			} else if (peek_next_token(f) == '=') {
				return compare_lte;
			}
			return compare_lt;
		}
		case '>': {
			if (peek_next_token(f) == '>') {
				if (peek_next_token(f, 2) == '=') {
					return assign_shift_right;
				}
				return shift_right;
			} else if (peek_next_token(f) == '=') {
				return compare_gte;
			}
			return compare_gt;
		}
		case '!': {
			if (peek_next_token(f) == '=') {
				return compare_neq;
			}
			return logical_not;
		}
		case '=': {
			if (peek_next_token(f) == '=') {
				return compare_eq;
			}
			return assign;
		}
		case '[': {
			return subscript;
		}
		case '(': return call;
		default: return not_an_operator;
	}
}

PrefixBindingPower get_binding_power_prefix(operators op) {
	switch (op) {
		case star:
		case ampersand:
		case minus:
		case plus:
			return PrefixBindingPower { .rhs = 180 };
		case bitwise_not:
			return PrefixBindingPower { .rhs = 170 };
		case logical_not:
			return PrefixBindingPower { .rhs = 110 };
		default: throw std::runtime_error(fmt::format("Invalid prefix operator {}", operator_to_string[op] ? operator_to_string[op] : "[help?]"));
	}
}

std::optional<InfixBindingPower> get_binding_power_infix(operators op) {
	switch (op) {
		case dot:
		case double_colon:
			return InfixBindingPower { .lhs = 200, .rhs = 201 };
		case shift_left:
		case shift_right:
			return InfixBindingPower { .lhs = 160, .rhs = 161 };
		case star:
		case divide:
		case mod:
			return InfixBindingPower { .lhs = 150, .rhs = 151 };
		case plus:
		case minus:
			return InfixBindingPower { .lhs = 140, .rhs = 141 };
		case ampersand:
		case bitwise_or:
		case bitwise_xor:
			return InfixBindingPower { .lhs = 130, .rhs = 131 };
		case compare_eq:
		case compare_neq:
		case compare_lt:
		case compare_lte:
		case compare_gt:
		case compare_gte:
			return InfixBindingPower { .lhs = 120, .rhs = 120 };
		case logical_and:
			return InfixBindingPower { .lhs = 100, .rhs = 101 };
		case logical_or:
			return InfixBindingPower { .lhs = 90, .rhs = 91 };
		case assign:
		case assign_add:
		case assign_sub:
		case assign_mul:
		case assign_div:
		case assign_mod:
		case assign_bitwise_and:
		case assign_bitwise_or:
		case assign_bitwise_xor:
		case assign_shift_left:
		case assign_shift_right:
			return InfixBindingPower { .lhs = 1, .rhs = 0 };
		default: return std::nullopt;
	}
}

std::optional<PostfixBindingPower> get_binding_power_postfix(operators op) {
	switch (op) {
		case subscript:
		case call:
			return PostfixBindingPower { .lhs = 190 };
		default: return std::nullopt;
	}
}

std::unique_ptr<AST::Expression> Parser::parse_pratt_expression(FILE* f, unsigned int min_bp) {
	std::unique_ptr<AST::Expression> lhs;

	if (currentToken < 0 || currentToken == '{') {
		// If known type rather than symbol for op
		lhs = parse_r_value(f);
	} else if (currentToken == '(') {
		get_next_token(f);
		lhs = parse_pratt_expression(f);
		EXPECT(')', "to match opening bracket");
	} else {
		auto op = parse_operator(f);
		advance_op(op, f);
		auto [rbp] = get_binding_power_prefix(op);
		auto rhs = parse_pratt_expression(f, rbp);
		lhs = std::make_unique<AST::UnaryExpressionPrefix>(op, std::move(rhs));
	}

	while (1) {
		auto op = parse_operator(f);
		if (op == not_an_operator) {
			break;
		}

		if (auto bp = get_binding_power_postfix(op)) {
			auto [lbp] = *bp;
			if (lbp < min_bp) {
				break;
			}
			advance_op(op, f);

			if (op == subscript) {
				auto rhs = parse_pratt_expression(f);
				EXPECT(']', "to match opening bracket");
				lhs = std::make_unique<AST::NewBinaryExpression>(op, std::move(lhs), std::move(rhs));
			} else if (op == call) {
				std::vector<std::unique_ptr<AST::Expression>> args;
				if (currentToken != ')')while (1) {
					args.push_back(parse_pratt_expression(f));
					if (currentToken == ')') {
						break;
					}
					EXPECT(',', "between args");
				}
				EXPECT(')', "to match opening bracket");
				lhs = std::make_unique<AST::NewCallExpression>(std::move(lhs), std::move(args));
			} else {
				lhs = std::make_unique<AST::UnaryExpressionPostfix>(op, std::move(lhs));
			}
			continue;
		}

		if (auto bp = get_binding_power_infix(op)) {
			auto [lbp, rbp] = *bp;
			if (lbp < min_bp) {
				break;
			}
			advance_op(op, f);

			auto rhs = parse_pratt_expression(f, rbp);

			auto ret = std::unique_ptr<AST::Expression>();

			auto desc = rhs.get();
			while (auto t = dynamic_cast<AST::NewBinaryExpression*>(desc)) {
				desc = t->lhs.get();
				if (t->op == logical_and) {
					continue;
				}
				auto [peek_lbp, peek_rbp] = get_binding_power_infix(t->op).value();
				if (peek_lbp == rbp) {
					throw std::runtime_error("infix operator with same binding power");
				} else {
					break;
				}
			}

			lhs = ret ? std::move(ret) : std::make_unique<AST::NewBinaryExpression>(op, std::move(lhs), std::move(rhs));

			continue;
		}

		break;
	}

	return lhs;
}

#include "../memory/gep.hpp"

AST::TypeInstance AST::UnaryExpressionPrefix::get_prefix_op_ret_type(operators op, std::unique_ptr<AST::Expression>& expr) {
	if (op == operators::ampersand) return expr->getType().get_pointer_to();
	else if (op == operators::star) return expr->getType().get_pointed_to();
	else return expr->getType();
}

#include "../types/type_base.hpp"

AST::TypeInstance AST::NewBinaryExpression::get_binop_ret_type(operators op, std::unique_ptr<AST::Expression>& lhs, std::unique_ptr<AST::Expression>& rhs) {
	if (op == double_colon) {
		AST::Namespace* lhs_ns;
		if (lhs->getType() == AST::get_fundamental_type("namespace_ref")) lhs_ns = (AST::Namespace*)lhs->codegen(nullptr);
		else lhs_ns = &*(lhs->getType().base_type);

		auto rhs_ref = dynamic_cast<AST::Reference*>(&*rhs);
		auto v = lhs_ns->get_var(rhs_ref->name);
		if (v) {
			rhs_ref->add_var(v);
			return v->underlying_type;
		}

		auto ns = lhs_ns->get_namespace_by_name(rhs_ref->name);
		if (ns) {
			rhs_ref->add_namespace(ns);
			return AST::get_fundamental_type("namespace_ref");
		}

		throw std::runtime_error(fmt::format("couldn't find {} in {}", rhs_ref->name, lhs_ns->get_name()));
	}

	if (op == dot) {
		auto rhs_ref = dynamic_cast<AST::Reference*>(&*rhs);

		auto v = lhs->getType().base_type->get_var(rhs_ref->name);
		if (v) {
			rhs_ref->add_var(v);
			return v->underlying_type;
		}

		auto field_info = lhs->getType().get_field_info_by_name(rhs_ref->get_name());

		if (std::holds_alternative<AST::FieldInfoField>(field_info)) {
			auto gep = std::make_shared<AST::GEP>(&*lhs, std::get<AST::FieldInfoField>(field_info).index);
			rhs_ref->add_var(gep);
			return std::get<AST::FieldInfoField>(field_info).type;
		} else {
			auto const_info = std::get<AST::FieldInfoConst>(field_info);
			
		}
	}

	if (op == subscript) {
		if (!lhs->getType().is_array()) throw std::runtime_error("Cannot subscript a non-array");
		return lhs->getType().get_array_elem_of();
	}

	return lhs->getType();
}

llvm::Value* AST::NewBinaryExpression::codegen(llvm::IRBuilder<> *builder, std::string name) {
	auto LHS_value = lhs->codegen(builder);
	auto RHS_value = rhs->codegen(builder);

	if (op == dot) {
		auto rhs_ref = dynamic_cast<AST::Reference*>(&*rhs);

		return rhs_ref->codegen(builder);
	}

	if (op == double_colon) {
		return rhs->codegen(builder);
	}

	if (op == assign) {
		return builder->CreateStore(RHS_value, lhs->codegen_to_ptr(builder));
	}

	switch (op) {
		case plus:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFAdd(LHS_value, RHS_value, name);
			else return builder->CreateAdd(LHS_value, RHS_value, name);
		case minus:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFSub(LHS_value, RHS_value, name);
			else return builder->CreateSub(LHS_value, RHS_value, name);
		case star:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFMul(LHS_value, RHS_value, name);
			else return builder->CreateMul(LHS_value, RHS_value, name);
		case divide:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFDiv(LHS_value, RHS_value, name);
			else return builder->CreateUDiv(LHS_value, RHS_value, name);
		case mod:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFRem(LHS_value, RHS_value, name);
			else return builder->CreateURem(LHS_value, RHS_value, name);

		case shift_left: return builder->CreateShl(LHS_value, RHS_value, name);
		case shift_right: return builder->CreateAShr(LHS_value, RHS_value, name);

		case ampersand: return builder->CreateAnd(LHS_value, RHS_value, name);
		case bitwise_or: return builder->CreateOr(LHS_value, RHS_value, name);

		case compare_eq:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFCmpUEQ(LHS_value, RHS_value, name);
			else return builder->CreateICmpEQ(LHS_value, RHS_value, name);
		case compare_neq:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFCmpUNE(LHS_value, RHS_value, name);
			else return builder->CreateICmpNE(LHS_value, RHS_value, name);
		case compare_lt:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFCmpULT(LHS_value, RHS_value, name);
			else return builder->CreateICmpULT(LHS_value, RHS_value, name);
		case compare_lte:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFCmpULE(LHS_value, RHS_value, name);
			else return builder->CreateICmpULE(LHS_value, RHS_value, name);
		case compare_gt:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFCmpUGT(LHS_value, RHS_value, name);
			else return builder->CreateICmpUGT(LHS_value, RHS_value, name);
		case compare_gte:
			if (LHS_value->getType()->isFloatingPointTy()) return builder->CreateFCmpUGE(LHS_value, RHS_value, name);
			else return builder->CreateICmpUGE(LHS_value, RHS_value, name);
		case subscript: 
			return builder->CreateLoad(this->codegen_to_ptr(builder));
		default: throw std::runtime_error(fmt::format("unimplemented binary op {}", operator_to_string[op]));
	}
}

llvm::Value* AST::UnaryExpressionPrefix::codegen(llvm::IRBuilder<> *builder, std::string name) {
	auto value = expr->codegen(builder);
	switch (op) {
		case plus:
			return value;
		case minus:
			if (value->getType()->isFloatingPointTy()) return builder->CreateFNeg(value);
			else return builder->CreateNeg(value);
		case star:
			return builder->CreateLoad(value);
		case ampersand: {
			auto expr_mem = dynamic_cast<AST::Reference*>(&*expr);
			if (!expr_mem) throw std::runtime_error("Cannot get reference to non-referemce");
			return expr_mem->codegen_to_ptr(builder);
		}
		default: throw std::runtime_error(fmt::format("unimplemented prefix op {}", operator_to_string[op]));
	}
}

llvm::Value* AST::UnaryExpressionPostfix::codegen(llvm::IRBuilder<> *builder, std::string name) {
	throw std::runtime_error(fmt::format("unimplemented postfix op {}", operator_to_string[op]));
}

#include "../types/type_base.hpp"

llvm::Value* AST::NewCallExpression::codegen(llvm::IRBuilder<> *builder, std::string name) {
	if (!(func->getType().base_type == AST::get_fundamental_type("function").base_type)) throw std::runtime_error(fmt::format("callee is not a function - cannot call a {}", func->getType().base_type->get_name()));
	std::vector<llvm::Value*> fargs = std::vector<llvm::Value*>();
	auto codegen_func = func->codegen(builder);

	if (auto lhs_dot_op = dynamic_cast<AST::NewBinaryExpression*>(&*func)) if (lhs_dot_op->op == dot && static_cast<llvm::Function*>(codegen_func)->hasFnAttribute("member_func")) {
		fargs.push_back(lhs_dot_op->lhs->codegen_to_ptr(builder));
	}

	for (auto &arg: args) {
		auto aV = arg->codegen(builder);
		fargs.push_back(aV);
	}

	auto llvm_function_ty = static_cast<llvm::FunctionType*>(codegen_func->getType()->getPointerElementType());
	return builder->CreateCall(llvm_function_ty, codegen_func, fargs);
}
