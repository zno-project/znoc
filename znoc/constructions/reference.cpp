#include "reference.hpp"
#include <fmt/format.h>
#include <deque>
#include <map>
#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include "../macros.hpp"
#include "../main.hpp"
#include "../memory/variable.hpp"
#include "../memory/memory_ref.hpp"
#include "../memory/gep.hpp"
#include "binary_op.hpp"
#include "construction_parse.hpp"
#include "function_call.hpp"
#include "../parsing.hpp"

/*std::unique_ptr<AST::ASTRef> parseStructRef(std::unique_ptr<AST::ASTRef> v) {
	if (currentToken != '.') return v;
	get_next_token(); // Trim .
	if (currentToken != tok_identifier) throw std::runtime_error("Expected to index struct with field name");
	std::string fieldName = *std::get_if<std::string>(&currentTokenVal);
	get_next_token(); // Trim field name
	return parseStructRef(std::make_unique<AST::StructFieldRef>(std::move(v), fieldName));
}*/

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

		/*if (currentToken != ')') {
			while (1) {
				args.push_back(parse_binary_expression(f));

				if (currentToken == ')') break;
				if (currentToken != ',') throw UNEXPECTED_CHAR(currentToken, ", after argument");
				get_next_token(f); // Trim ,
			}
		}
		
		get_next_token(f); // Trim )*/
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

		/*if (parsed_namespace.next_token == '.') while (1) {
			get_next_token(f); // Trim `.`
			auto field_name = std::get<std::string>(currentTokenVal);
			get_next_token(f); // Trim field name

			auto field_info = variable->underlying_type.get_field_info_by_name(field_name);
			variable = std::make_unique<AST::GEP>(variable, field_info.index);

			if (currentToken != '.') break;
		}*/

		return std::make_unique<AST::MemoryRef>(variable);
	}

	/*if (currentToken != '(') {
		return std::make_unique<AST::VariableRef>(identifier, fields);; // Make variable reference if not function call
	}*/
	/*if (currentToken == '.') {
		std::cout << "struct ref" << std::endl;
		return parseStructRef(std::move(v));
	}*/

	/*std::string fullFuncLookupName;
	fullFuncLookupName += fields.size() > 0 ? "" : identifier;*/

	/*if (fields.size() > 0) {
		// Get type of last field
		// Then prefix to the func name
		auto v = AST::get_var(identifier);
		auto ty = v->getVarType();

		auto funcN = fields.back();
		fields.pop_back(); // Remove function name from lookup

		auto idxs = ty.get_field_idxs(fields);
		for (auto &idx: idxs) {
			ty = ty->fieldTypes[idx];
		}
		fullFuncLookupName += ty->name + "::" + funcN;
	}*/

	/*std::cout << "full internal func name is " << fullFuncLookupName << std::endl;

	get_next_token(f); // Trim (
	std::vector<std::unique_ptr<AST::Expression>> args;
	
	if (currentToken != ')') while (1) {
		args.push_back(parse_binary_expression(f));

		if (currentToken == ')') break;
		if (currentToken != ',') throw UNEXPECTED_CHAR(currentToken, ", after argument");
		get_next_token(f); // Trim ,
	}
	
	get_next_token(f); // Trim )
	return std::make_unique<AST::FunctionCall>(fullFuncLookupName, std::move(args));*/
}