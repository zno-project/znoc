#include "attributes.hpp"
#include "parsing.hpp"
#include "macros.hpp"
#include <exception>

attributes_t currentAttributes;

Attributes tok_to_attribute(FILE* f) {
	//if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "attribute name");
	auto attr = EXPECT_IDENTIFIER("attribute name");
	if (attr == "inline") return Attributes::AlwaysInline;
	else if (attr == "extern") return Attributes::NoMangle;
	else throw std::runtime_error(fmt::format("unknown attribute {}", attr));
}

void Parser::parse_attributes(FILE* f) {
	EXPECT('#', "to start attribute list");
	//if (currentToken != '#') throw UNEXPECTED_CHAR(currentToken, "`#[` to start attribute list");
	//get_next_token(f);
	LIST('[', ',', ']', {
		auto attr = tok_to_attribute(f);
		currentAttributes[(unsigned long)attr] = 1;
	}, "attribute list");
	/*if (currentToken != '[') throw UNEXPECTED_CHAR(currentToken, "`#[` to start attribute list");
	get_next_token(f);
	if (currentToken != ']') while (1) {
		auto attr = tok_to_attribute(f);
		currentAttributes[(unsigned long)attr] = 1;
		get_next_token(f);
		if (currentToken != ',') break;
		get_next_token(f); 
	}
	if (currentToken != ']') throw UNEXPECTED_CHAR(currentToken, "`]` to end attribute list");
	get_next_token(f);*/
}

void Parser::clear_attributes() {
	currentAttributes = 0;
}
