#include "attributes.hpp"
#include "parsing.hpp"
#include "macros.hpp"
#include <exception>
#include <iostream>

attributes_t Parser::parse_attributes(zno_ifile& f) {
	EXPECT(tok_attributes, "to start attribute list");

	attributes_t attributes;

	LIST('[', ',', ']', {
		auto attr_name = EXPECT_IDENTIFIER("attribute name");
		if (attr_name == "extern") attributes.extern_ = true;
		else if (attr_name == "always_inline") attributes.always_inline = true;
		else if (attr_name == "mangle_name") {
			EXPECT('(', "attributes arg");
			attributes.mangle_name = EXPECT_IDENTIFIER("mangle_name");
			EXPECT(')', "attributes arg");
		}
	}, "attribute list");

	return attributes;
}
