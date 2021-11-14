#include "pratt_parser.hpp"
#include "expression.hpp"
#include "../parsing.hpp"
#include <memory>
#include <stdio.h>

char *operator_to_string[] = {".", "::", "+", "-", "*", "/", "%", "&", "|", "~", "^", "&&", "||", "!", "<<", ">>", "==", "!=", "<", ">", "<=", ">=", "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=", "is", "[]"};

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
			get_next_token(f);
			break;
		case double_colon:
		case logical_and:
		case logical_or:
		case shift_left:
		case shift_right:
		case compare_eq:
		case compare_neq:
		case compare_lt:
		case compare_lte:
		case compare_gt:
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
		default: return not_an_operator;
	}
}

PrefixBindingPower get_binding_power_prefix(operators op) {
	switch (op) {
		case star:
		case ampersand:
		case minus:
		case plus:
			return PrefixBindingPower { .rhs = 190 };
		case bitwise_not:
			return PrefixBindingPower { .rhs = 170 };
		case logical_not:
			return PrefixBindingPower { .rhs = 110 };
		default: throw std::runtime_error("Invalid prefix operator");
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
			return PostfixBindingPower { .lhs = 180 };
		default: return std::nullopt;
	}
}

std::unique_ptr<AST::Expression> Parser::parse_pratt_expression(FILE* f, unsigned int min_bp) {
	std::unique_ptr<AST::Expression> lhs;

	if (currentToken < 0) {
		// If known type rather than symbol for op
		lhs = parse_r_value(f);
	} else if (currentToken == '(') {
		get_next_token(f);
		lhs = parse_pratt_expression(f);
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

			lhs = std::make_unique<AST::UnaryExpressionPostfix>(op, std::move(lhs));
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
