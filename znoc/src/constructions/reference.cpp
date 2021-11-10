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
#include "../memory/struct_const.hpp"
#include "../memory/gep.hpp"
#include "binary_op.hpp"
#include "unary_op.hpp"
#include "construction_parse.hpp"
#include "function_call.hpp"
#include "../parsing.hpp"
#include "struct_init.hpp"

// IDENTIFIER EXPRESSION
// identifier_expr = identifier |
//                   identifier '(' (binary_expr ',')* ')';
std::unique_ptr<AST::Expression> Parser::parse_identifier_expression(FILE* f) {
	std::vector<std::string> fields;

	auto parsed_namespace = Parser::parse_namespace(f);

	if (parsed_namespace.next_token == '(') { // Function call
		auto func_name = parsed_namespace.next_identifier;


		std::vector<std::unique_ptr<AST::Expression>> args;

		LIST('(', ',', ')', {
			args.push_back(parse_binary_expression(f));
		}, "function args");

		return std::make_unique<AST::FunctionCall>(parsed_namespace.parsed_namespace->get_function_by_name(func_name), std::move(args));
	} else if (parsed_namespace.next_token == '{' || parsed_namespace.next_token == '<') { // Aggregate type init
		auto type_name = parsed_namespace.next_identifier;
		return parse_struct_init(f, parsed_namespace.parsed_namespace->get_type_by_name(type_name));
	} else {
		std::cout << "Variable reference or member func - ignoring namespaces" << std::endl;
		std::shared_ptr<AST::MemoryLoc> variable = AST::get_var(parsed_namespace.next_identifier);

		UNTIL_NOT('.', {
			auto field_name = EXPECT_IDENTIFIER("field name");
			if (currentToken == '(') {
				// Call to member function
				std::vector<std::unique_ptr<AST::Expression>> args;
				// TODO: This could probably be improved
				args.push_back(std::make_unique<AST::UnaryExpression>('&', std::make_unique<AST::MemoryRef>(variable)));

				LIST('(', ',', ')', {
					args.push_back(parse_binary_expression(f));
				}, "function args");

				return std::make_unique<AST::FunctionCall>(variable->underlying_type.get_function_by_name(field_name), std::move(args));
			} else {
				// Variable ref
				auto field_info = variable->underlying_type.get_field_info_by_name(field_name);

				if (std::holds_alternative<AST::FieldInfoField>(field_info)) {
					variable = std::make_unique<AST::GEP>(variable, std::get<AST::FieldInfoField>(field_info).index);
				} else {
					auto constant = std::get<AST::FieldInfoConst>(field_info);
					variable = std::make_unique<AST::StructConstAccess>(constant.linkage_name, constant.type);
				}
			}
		});

		return std::make_unique<AST::MemoryRef>(variable);
	}
}
