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

int currentToken;
std::variant<std::string, double> currentTokenVal;

zno_ifile::operator FILE*() { return f; }

zno_ifile::zno_ifile(std::string path) {
	this->path = path;
	f = fopen(path.c_str(), "r");
}

zno_ifile::~zno_ifile() {
	fclose(f);
}

std::string zno_ifile::get_path() { return path; }

int advance(zno_ifile& f) {
	int LastChar = fgetc(f);

	if (LastChar == '\n' || LastChar == '\r') {
		LexLoc.Line++;
		LexLoc.Col = 0;
	} else LexLoc.Col++;
	return LastChar;
}

std::map<std::string, char> lastChars;

int get_token(zno_ifile& f) {
	if (lastChars[f.get_path()] == 0) lastChars[f.get_path()] = advance(f);
	char& lastChar = lastChars[f.get_path()];

	// skip whitespace
	while (isspace(lastChar)) lastChar = advance(f);

	if (lastChar == '"') {
		std::string s;

		while (true) {
			lastChar = advance(f);
			if (lastChar == '\\') {
				char escapedChar = advance(f);
				switch (escapedChar) {
					case 'a': s += '\a'; break;
					case 'b': s += '\b'; break;
					case 'f': s += '\f'; break;
					case 'n': s += '\n'; break;
					case 'r': s += '\r'; break;
					case 't': s += '\t'; break;
					case 'v': s += '\v'; break;
					default: s += escapedChar; break;
				}
				continue;
			}
			if (lastChar == '"') break;
			s += lastChar;
		}

		lastChar = advance(f);
		currentTokenVal = s;
		return tok_string;
	}

	if (isalpha(lastChar) | (lastChar == '_')) {
		std::string identifier;
		identifier += lastChar;

		// Get rest of identifier - can have numbers after first char
		
		while (isalnum(lastChar = advance(f)) || (lastChar == '_')) identifier += lastChar;

		if (identifier == "func") return tok_func;
		else if (identifier == "let") return tok_let;
		else if (identifier == "const") return tok_const;
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
		else if (identifier == "attributes") return tok_attributes;
		else if (identifier == "extension") return tok_extension;
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
		if (foundDecimal) {
			return tok_decimal_numeric_literal;
		} else {
			return tok_integer_numeric_literal;
		}
	}

	if (lastChar == EOF) return tok_eof;

	int thisChar = lastChar;
	lastChar = advance(f);
	return thisChar;
}

int peek_next_token(zno_ifile& f, int offset) {
	auto currentPos = ftell(f);
	char oldLastChar = lastChars[f.get_path()];
	auto oldLexLoc = LexLoc;
	auto oldCurrentToken = currentToken;
	auto oldCurrentTokenVal = currentTokenVal;
	int ret = 0;
	for (int i = 0; i < offset; i++) {
		ret = get_next_token(f);
	}
	lastChars[f.get_path()] = oldLastChar;
	LexLoc = oldLexLoc;
	fseek(f, currentPos, SEEK_SET);
	currentToken = oldCurrentToken;
	currentTokenVal = oldCurrentTokenVal;
	return ret;
}

int get_next_token(zno_ifile& f) {
	currentToken = get_token(f);
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
	std::shared_ptr<AST::Namespace> &current_namespace,
	__attribute__((unused)) bool insideUses) {
	searchedIncludes.push_back(path);

	zno_ifile f(path.string());

	get_next_token(f);
	attributes_t attributes;

	while (1) {
		switch (currentToken) {
			case tok_attributes: {
				attributes = Parser::parse_attributes(f);
				break;
			}
			case tok_eof: {
				attributes = attributes_t();
				return 0;
			}
			case tok_uses: {
				get_next_token(f);
				
				EXPECT('\'', "\' for path to included file");
				
				std::string filePath;
				UNTIL('\'', {
					if (currentToken == tok_identifier) filePath += std::get<std::string>(currentTokenVal);
					else filePath += currentToken;
					get_next_token(f);
				});

				auto newP = path;
				newP.replace_filename(filePath.c_str());

				std::string namespace_name = newP.stem().string();

				OPTIONAL(tok_as, {
					namespace_name = EXPECT_IDENTIFIER("after `as` statement");
				});

				if (has_searched_path(newP)) {
					std::cout << "Already compiling " << newP.string() << std::endl;
					break;
				}

				std::cout << "Parsing include " << newP.string() << " as " << namespace_name << std::endl;
				auto new_namespace = std::make_shared<AST::Namespace>(namespace_name);

				int old_current_token = currentToken;

				*current_namespace << new_namespace;

				parse_file(newP, new_namespace, true);

				currentToken = old_current_token;

				attributes = attributes_t();
				break;
			}
			case tok_struct:
			case tok_class: {
				*current_namespace << Parser::parse_aggregate_type_definition(f);
				attributes = attributes_t();
				break;
			}
			case tok_extension: {
				Parser::parse_extension(f);
				attributes = attributes_t();
				break;
			}
			case tok_func: {
				*current_namespace << Parser::parse_function(f, attributes);
				attributes = attributes_t();
				break;
			}
			case tok_typedef: {
				Parser::parse_typedef(f, *current_namespace);
				attributes = attributes_t();
				break;
			}
			default: {
				throw std::runtime_error(fmt::format("Unimplemented {}", currentToken));
			}
		}
	}
}
