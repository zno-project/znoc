#include "attributes.hpp"
#include "parsing.hpp"
#include "macros.hpp"
#include <exception>
#include <iostream>

Attributes::Attributes tok_to_attribute(zno_ifile& f) {
	auto attr = EXPECT_IDENTIFIER("attribute name");
	if (attr == "inline") return Attributes::AlwaysInline;
	else if (attr == "extern") return Attributes::Extern;
	else throw std::runtime_error(fmt::format("unknown attribute {}", attr));
}

attributes_t Parser::parse_attributes(zno_ifile& f) {
	EXPECT(tok_attributes, "to start attribute list");

	attributes_t attributes = 0;

	LIST('[', ',', ']', {
		auto attr = tok_to_attribute(f);
		attributes[attr] = 1;
	}, "attribute list");

	return attributes;
}
