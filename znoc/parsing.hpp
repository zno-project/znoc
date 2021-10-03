#ifndef _PARSING_H
#define _PARSING_H

#include <stdio.h>
#include <variant>
#include <string>
#include <filesystem>
#include "location.hpp"

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
	tok_numeric_literal = -6,

	// typedef
	tok_struct = -7,

	// control flow
	tok_if = -8,
	tok_else = -9,
	tok_for = -10,
	tok_throw = -11,
	tok_switch = -12,
	tok_case = -13,
	tok_break = -14,
	tok_continue = -15,
	tok_fallthrough = -16,
	tok_default = -17,
	tok_while = -18,

	tok_uses = -19,

	tok_class = -20
};

#include "constructions/namespace.hpp"

int parse_file(std::filesystem::path p,
	std::unique_ptr<AST::Namespace> &current_namespace,
	bool insideUses = false);

#endif