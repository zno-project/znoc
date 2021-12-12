#include "attributes.hpp"
#include "parsing.hpp"
#include "macros.hpp"
#include <exception>

attributes_t currentAttributes;

Attributes tok_to_attribute(FILE* f) {
	auto attr = EXPECT_IDENTIFIER("attribute name");
	if (attr == "inline") return Attributes::AlwaysInline;
	else if (attr == "extern") return Attributes::NoMangle;
	else throw std::runtime_error(fmt::format("unknown attribute {}", attr));
}

void Parser::parse_attributes(FILE* f) {
	EXPECT('#', "to start attribute list");

	LIST('[', ',', ']', {
		auto attr = tok_to_attribute(f);
		currentAttributes[(unsigned long)attr] = 1;
	}, "attribute list");
}

void Parser::clear_attributes() {
	currentAttributes = 0;
}
