#ifndef _PARSING_H
#define _PARSING_H

#include <stdio.h>
#include <variant>
#include <string>
#include <filesystem>
#include "macros.hpp"

int advance(FILE* f);
int get_token(FILE* f);
int get_next_token(FILE* f);

extern int currentToken;
extern std::variant<std::string, double> currentTokenVal;

enum Token {
	tok_eof = -1,

	// commands
	tok_func = -2,
	tok_let = -3,
	tok_return = -4,

	// primary
	tok_identifier = -5,
	tok_integer_numeric_literal = -6,
	tok_decimal_numeric_literal = -7,

	// typedef
	tok_struct = -8,

	// control flow
	tok_if = -9,
	tok_else = -10,
	tok_for = -11,
	tok_throw = -12,
	tok_switch = -13,
	tok_case = -14,
	tok_break = -15,
	tok_continue = -16,
	tok_fallthrough = -17,
	tok_default = -18,
	tok_while = -19,

	tok_uses = -20,

	tok_class = -21,
	tok_as = -22,
	tok_typedef = -23
};

#include "constructions/namespace.hpp"

int parse_file(std::filesystem::path p,
	std::unique_ptr<AST::Namespace> &current_namespace,
	bool insideUses = false);

#define EXPECT(tok, err) { \
	if (currentToken != tok) throw UNEXPECTED_CHAR(currentToken, #tok " " err);	\
	get_next_token(f);	\
}

#define EXPECT_IDENTIFIER(err) ({ \
	if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, err);	\
	auto ret = std::get<std::string>(currentTokenVal);	\
	get_next_token(f);	\
	ret;	\
})

#define EXPECT_NUMERIC_LITERAL(err) ({ \
	if (currentToken != tok_decimal_numeric_literal && currentToken != tok_integer_numeric_literal) throw UNEXPECTED_CHAR(currentToken, err);	\
	auto contains_dp = currentToken == tok_decimal_numeric_literal;	\
	auto ret = std::get<double>(currentTokenVal);	\
	get_next_token(f);	\
	std::make_tuple(ret, contains_dp);	\
})

#define UNTIL(end, body) {	\
	while (currentToken != end) {	\
		body	\
	}	\
	get_next_token(f);	\
}

#define UNTIL_NOT_AT_LEAST_ONCE(cont, body) {	\
	do {	\
		get_next_token(f);	\
		body	\
	} while (currentToken == cont);	\
}

#define UNTIL_NOT(cont, body) {	\
	while (currentToken == cont) {	\
		get_next_token(f);	\
		body	\
	}	\
}

#define LIST(start, separator, end, body, list_name) { \
	EXPECT(start, "to start " list_name);	\
	if (currentToken != end) while (1) {	\
		body	\
		if (currentToken != separator) break;	\
		get_next_token(f);	\
	}	\
	EXPECT(end, "to end " list_name);	\
}

#define OPTIONAL_LIST(start, separator, end, body, list_name) { \
	OPTIONAL(start, {	\
		if (currentToken != end) while (1) {	\
			body	\
			if (currentToken != separator) break;	\
			get_next_token(f);	\
		}	\
		EXPECT(end, "to end " list_name);	\
	});	\
}

#define IF_TOK_ELSE_IDENTIFIER(id_var, body_if, body_else) {	\
	if (currentToken == tok_identifier) {	\
		auto id_var = std::get<std::string>(currentTokenVal);	\
		get_next_token(f);	\
		body_if	\
	} else {	\
		body_else \
	}	\
}

#define IF_TOK_ELSE(tok, body_if, body_else) {	\
	if (currentToken == tok) {	\
		get_next_token(f);	\
		body_if	\
	} else {	\
		body_else \
	}	\
}

#define OPTIONAL(tok, body) {	\
	IF_TOK_ELSE(tok, body, {})	\
}

#endif