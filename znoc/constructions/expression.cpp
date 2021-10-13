#include "expression.hpp"
#include "../parsing.hpp"
#include "../macros.hpp"
#include "construction_parse.hpp"

#include <iostream>

llvm::BasicBlock *mergeBB = nullptr; // Merge block of current control flow codegen
llvm::BasicBlock *condBB = nullptr; // Condition block of current control flow codegen

// PARENTHESIS EXPRESSION
// parenthesis_expr = '(' binary_expr ')';
std::unique_ptr<AST::Expression> Parser::parse_parentheses_expression(FILE* f) {
	get_next_token(f); // Trim (
	auto E = parse_binary_expression(f);
	if (currentToken != ')') throw UNEXPECTED_CHAR(currentToken, ") to match opening (");
	get_next_token(f); // Trim )
	return E;
}

// NON SEMICOLON STATEMENT
// non_semicolon_statement = codeblock |
//                           while_loop |
//                           switch |
//                           if;
std::unique_ptr<AST::Expression> Parser::parse_non_semicolon_statement(FILE* f) {
	switch (currentToken) {
		case '{': {
			get_next_token(f);
			return parse_code_block(f);
		}
		case tok_while: {
			return parse_while(f);
		}
		case tok_switch: {
			return parse_switch(f);
		}
		case tok_if: {
			return parse_if_def(f);
		}
		default:
			return nullptr;
	}
}

// R-VALUE
// r_value = non_semicolon_statement |
//           identifier_expr |
//           numeric_const |
//           variable_def |
//           parenthesis_expr;
std::unique_ptr<AST::Expression> Parser::parse_r_value(FILE* f) {
	auto non_semicolon_statement = parse_non_semicolon_statement(f);
	if (non_semicolon_statement) return non_semicolon_statement;

	switch (currentToken) {
		case tok_identifier: {
			return parse_identifier_expression(f);
		}
		case tok_numeric_literal: {
			return parse_numeric_literal(f);
		}
		case tok_let: {
			//throw std::runtime_error("variable def");
			return parse_variable_def(f);
		}
		case '(': {
			return parse_parentheses_expression(f);
		}
		default: throw UNEXPECTED_CHAR(currentToken, "r-value");
	}
}

// SEMICOLON STATEMENT
// semicolon_statement = break |
//                       return |
//                       fallthrough;
std::unique_ptr<AST::Expression> Parser::parse_semicolon_statement(FILE* f) {
	switch (currentToken) {
		case tok_break: {
			return parse_break(f);
		}
		case tok_continue: {
			return parse_continue(f);
		}
		case tok_return: {
			return parse_return(f);
		}
		case tok_fallthrough: {
			return parse_fallthrough(f);
		}
		default: return nullptr;
	}
}

// STATEMENT
// statement = non_semicolon_statement | semicolon_statement ';' | binary_expr ';' ;
std::unique_ptr<AST::Expression> Parser::parse_statement(FILE* f) {
	auto statement = parse_non_semicolon_statement(f);
	if (statement) return statement;

	statement = parse_semicolon_statement(f);
	if (statement) {
		if (currentToken != ';') throw UNEXPECTED_CHAR(currentToken, "; after statement");
		get_next_token(f); // trim ;
		return statement;
	}

	statement = parse_binary_expression(f);
	if (currentToken != ';') throw UNEXPECTED_CHAR(currentToken, "; after statement");
	get_next_token(f); // trim ;
	return statement;
}