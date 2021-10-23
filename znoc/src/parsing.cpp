#include "parsing.hpp"

#include "constructions/expression.hpp"
#include "constructions/function_def.hpp"
#include "types/type.hpp"
#include "attributes.hpp"
#include "types/type.hpp"
#include "constructions/typedef.hpp"
#include "macros.hpp"
#include "location.hpp"
#include <fmt/format.h>

#include <stdio.h>
#include <variant>
#include <iostream>
#include <string>
#include <filesystem>

//FILE *f;
int currentToken;
std::variant<std::string, double> currentTokenVal;

int advance(FILE *f) {
	//std::cout << "advance" << std::endl;
	int LastChar = fgetc(f);

	if (LastChar == '\n' || LastChar == '\r') {
		LexLoc.Line++;
		LexLoc.Col = 0;
	} else LexLoc.Col++;
	if (LastChar == ')') {
		__asm__("nop");
	}
	return LastChar;
}

int get_token(FILE *f) {
	//std::cout << "get_token" << std::endl;
	static std::map<FILE*, char> lastChars = std::map<FILE*, char>();
	if (lastChars[f] == 0) lastChars[f] = advance(f);
	char &lastChar = lastChars[f];
	//std::cout << "after advance should be called" << std::endl;

	// skip whitespace
	while (isspace(lastChar)) lastChar = advance(f);

	if (isalpha(lastChar) | (lastChar == '_')) {
		std::string identifier;
		identifier += lastChar;

		// Get rest of identifier - can have numbers after first char
		
		while (isalnum(lastChar = advance(f)) || (lastChar == '_')) identifier += lastChar;

		if (identifier == "func") return tok_func;
		else if (identifier == "let") return tok_let;
		else if (identifier == "struct") return tok_struct;
		else if (identifier == "return") return tok_return;
		else if (identifier == "if") return tok_if;
		else if (identifier == "else") return tok_else;
		else if (identifier == "for") return tok_for;
		else if (identifier == "throw") return tok_throw;
		else if (identifier == "switch") return tok_switch;
		else if (identifier == "case") return tok_case;
		else if (identifier == "break") return tok_break;
		else if (identifier == "continue") return tok_continue;
		else if (identifier == "fallthrough") return tok_fallthrough;
		else if (identifier == "default") return tok_default;
		else if (identifier == "while") return tok_while;
		else if (identifier == "uses") return tok_uses;
		else if (identifier == "class") return tok_class;
		else if (identifier == "as") return tok_as;
		else if (identifier == "typedef") return tok_typedef;
		else {
			currentTokenVal = identifier;
			return tok_identifier;
		}
	}

	if (isdigit(lastChar)) {
		bool foundDecimal = false;
		std::string number;

		do {
			number += lastChar;
			if (lastChar == '.') foundDecimal = true;
			lastChar = advance(f);
		} while (isdigit(lastChar) || (!foundDecimal && lastChar == '.'));

		currentTokenVal = strtod(number.c_str(), nullptr);
		return tok_numeric_literal;
	}

	if (lastChar == EOF) return tok_eof;

	int thisChar = lastChar;
	lastChar = advance(f);
	return thisChar;
}

int get_next_token(FILE *f) {
	//std::cout << "getnexttoken" << std::endl;
	currentToken = get_token(f);
	std::cout << "Tok: " << (char)currentToken << "(" << currentToken << ")" << std::endl;
	return currentToken;
}

std::vector<std::filesystem::path> searchedIncludes;

bool has_searched_path(std::filesystem::path p) {
	for (auto &path : searchedIncludes) {
		if (std::filesystem::equivalent(path, p)) return true;
	}
	return false;
}

int parse_file(std::filesystem::path path,
	std::unique_ptr<AST::Namespace> &current_namespace,
	bool insideUses) {
	searchedIncludes.push_back(path);

	FILE *f;
	if ((f = fopen(path.string().c_str(), "r")) == NULL) return 1;

	get_next_token(f);
	while (1) {
		std::cout << "current token is " << currentToken << std::endl;
		switch (currentToken) {
			case '#': {
				Parser::parse_attributes(f);
				break;
			}
			case tok_eof: {
				fclose(f);
				return 0;
			}
			//case ';': get_next_token(); break;
			case tok_uses: {
				get_next_token(f);
				
				EXPECT('\'', "\' for path to included file");
				//if (currentToken != '\'') throw UNEXPECTED_CHAR(currentToken, "\' for path to included file");
				std::string filePath;
				UNTIL('\'', {
					if (currentToken == tok_identifier) filePath += std::get<std::string>(currentTokenVal);
					else filePath += currentToken;
					get_next_token(f);
				});
				/*while (1) {
					get_next_token(f);
					if (currentToken == '\'') break;
					else if (currentToken == tok_identifier) filePath += std::get<std::string>(currentTokenVal);
					else filePath += currentToken;
				}
				get_next_token(f);*/

				auto newP = path;
				newP.replace_filename(filePath.c_str());

				std::string namespace_name = newP.stem();

				OPTIONAL(tok_as, {
					namespace_name = EXPECT_IDENTIFIER("after `as` statement");
				});

				if (has_searched_path(newP)) {
					std::cout << "Already compiling " << newP.string() << std::endl;
					break;
				}

				std::cout << "Parsing include " << newP.string() << " as " << namespace_name << std::endl;
				auto new_namespace = std::make_unique<AST::Namespace>(namespace_name);

				int old_current_token = currentToken;
				parse_file(newP, new_namespace, true);
				std::cout << "Parsed include " << newP.string() << " as " << namespace_name << std::endl;
				*current_namespace << std::move(new_namespace);
				currentToken = old_current_token;

				Parser::clear_attributes();
				break;
			}
			case tok_struct:
			case tok_class: {
				*current_namespace << Parser::parse_aggregate_type_definition(f);
				Parser::clear_attributes();
				break;
			}
			/*default: {
				auto E = Parser::parse_non_semicolon_statement();

				if (E) {
					auto e = E.release();
					if (typeid(*e) == typeid(AST::Function)) {
						static_cast<AST::Function*>(e);
					} else {
						throw std::runtime_error(fmt::format("Cannot have {} as top level statement", typeid(e).name()));
					}
				} else {
					/*std::cout << "global var" << std::endl;
					auto var_def = Parser::parse_variable_def();
					if (currentToken != ';') throw UNEXPECTED_CHAR(currentToken, "; after statement");
					get_next_token(); // trim ;
					variableDeclarations.push_back(std::pair<std::unique_ptr<AST::VariableDef>, bool>(static_cast<AST::VariableDef*>(var_def.get()), insideUses));
					throw std::runtime_error("Unimplemented");
				}*/
			case tok_func: {
				*current_namespace << Parser::parse_function(f);
				Parser::clear_attributes();
				break;
			}
			case tok_typedef: {
				Parser::parse_typedef(f, *current_namespace);
				Parser::clear_attributes();
				break;
			}
			default: {
				throw std::runtime_error(fmt::format("Unimplemented {}", currentToken));
			}
		}
		Parser::clear_attributes();
	}
}