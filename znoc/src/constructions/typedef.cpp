#include "typedef.hpp"
#include "../parsing.hpp"
#include "../types/type.hpp"

void Parser::parse_typedef(FILE *f, AST::Namespace &n) {
	EXPECT(tok_typedef, "for type alias");
	auto type = parse_type(f);
	EXPECT(tok_as, "for type alias");
	auto new_name = EXPECT_IDENTIFIER("new name for type");
	n.add_type_with_name(type, new_name);
}
