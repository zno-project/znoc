#include "reference.hpp"
#include <fmt/format.h>
#include <deque>
#include <map>
#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include "../macros.hpp"
#include "../memory/variable.hpp"
#include "../memory/memory_ref.hpp"
#include "../memory/gep.hpp"
#include "binary_op.hpp"
#include "construction_parse.hpp"
#include "function_call.hpp"
#include "../parsing.hpp"

// IDENTIFIER EXPRESSION
// identifier_expr = identifier |
//                   identifier '(' (binary_expr ',')* ')';
std::unique_ptr<AST::Expression> Parser::parse_identifier_expression(FILE* f) {
	std::vector<std::string> fields;

	auto parsed_namespace = Parser::parse_namespace(f);

	if (parsed_namespace.next_token == '(') { // Function call
		auto func_name = parsed_namespace.next_identifier;
		std::cout << "call to func " << parsed_namespace.parsed_namespace->get_name() << ":" << func_name << "(...)" << std::endl;

		std::vector<std::unique_ptr<AST::Expression>> args;

		LIST('(', ',', ')', {
			args.push_back(parse_binary_expression(f));
		}, "function args");

		return std::make_unique<AST::FunctionCall>(parsed_namespace.parsed_namespace->get_function_by_name(func_name), std::move(args));
	} else {
		std::cout << "Variable reference - ignoring namespaces" << std::endl;
		std::cout << "Variable reference - current tok is " << currentToken << std::endl;
		std::shared_ptr<AST::MemoryLoc> variable = AST::get_var(parsed_namespace.next_identifier);

		UNTIL_NOT('.', {
			auto field_name = EXPECT_IDENTIFIER("field name");
			auto field_info = variable->underlying_type.get_field_info_by_name(field_name);
			variable = std::make_unique<AST::GEP>(variable, field_info.index);
		});

		return std::make_unique<AST::MemoryRef>(variable);
	}
}