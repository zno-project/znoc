#include "reference.hpp"
#include <fmt/format.h>
#include <deque>
#include <map>
#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include "../macros.hpp"
#include "../memory/variable.hpp"
#include "../memory/gep.hpp"
#include "construction_parse.hpp"
#include "../memory/memory_ref.hpp"
#include "function_call.hpp"
#include "../parsing.hpp"
#include "struct_init.hpp"
#include "../main.hpp"

// IDENTIFIER EXPRESSION
// identifier_expr = identifier |
//                   identifier '(' (binary_expr ',')* ')';
std::unique_ptr<AST::Expression> Parser::parse_identifier_expression(FILE* f) {
	auto id = EXPECT_IDENTIFIER("identifier");

	auto v = AST::get_var(id);
	if (v) return std::make_unique<AST::Reference>(v, id);
	
	auto n = GlobalNamespace->get_namespace_by_name(id);
	if (n) return std::make_unique<AST::Reference>(n, id);

	return std::make_unique<AST::Reference>(id);
}
