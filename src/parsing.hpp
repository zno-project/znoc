#ifndef _PARSING_H
#define _PARSING_H

#include <stdio.h>
#include <variant>
#include <string>
#include <filesystem>
#include "macros.hpp"

class zno_ifile {
	private:
	std::string path;
	FILE *f;
	zno_ifile(FILE *f, std::string path) : f(f), path(path) {};

	public:
	zno_ifile(const zno_ifile&) = delete;
	operator FILE*();
	zno_ifile(std::string path);
	~zno_ifile();
	std::string get_path();
};

int advance(zno_ifile& f);
int get_token(zno_ifile& f);
int peek_next_token(zno_ifile& f, int offset = 1);
int get_next_token(zno_ifile& f);

extern int currentToken;
extern std::variant<std::string, double> currentTokenVal;

enum Token {
	tok_eof = -__LINE__,

	// commands
	tok_func = -__LINE__,
	tok_let = -__LINE__,
	tok_const = -__LINE__,
	tok_return = -__LINE__,

	// primary
	tok_identifier = -__LINE__,
	tok_integer_numeric_literal = -__LINE__,
	tok_decimal_numeric_literal = -__LINE__,

	// typedef
	tok_struct = -__LINE__,

	// control flow
	tok_if = -__LINE__,
	tok_else = -__LINE__,
	tok_for = -__LINE__,
	tok_throw = -__LINE__,
	tok_switch = -__LINE__,
	tok_case = -__LINE__,
	tok_break = -__LINE__,
	tok_continue = -__LINE__,
	tok_fallthrough = -__LINE__,
	tok_default = -__LINE__,
	tok_while = -__LINE__,

	tok_uses = -__LINE__,

	tok_class = -__LINE__,
	tok_as = -__LINE__,
	tok_typedef = -__LINE__,

	tok_string = -__LINE__,

	tok_attributes = -__LINE__
};

namespace AST { class Namespace; }

int parse_file(std::filesystem::path p,
	std::shared_ptr<AST::Namespace> &current_namespace,
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
