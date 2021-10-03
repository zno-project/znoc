#include "reference.hpp"
#include <fmt/format.h>
#include <deque>
#include <map>
#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include "../macros.hpp"
#include "../main.hpp"
#include "variable.hpp"
#include "binary_op.hpp"
#include "function.hpp"
#include "../parsing.hpp"

/*std::unique_ptr<AST::ASTRef> parseStructRef(std::unique_ptr<AST::ASTRef> v) {
	if (currentToken != '.') return v;
	get_next_token(); // Trim .
	if (currentToken != tok_identifier) throw std::runtime_error("Expected to index struct with field name");
	std::string fieldName = *std::get_if<std::string>(&currentTokenVal);
	get_next_token(); // Trim field name
	return parseStructRef(std::make_unique<AST::StructFieldRef>(std::move(v), fieldName));
}*/

enum class IdentifierPart {
	Namespace,
	FieldContainer,
	Function
};

// IDENTIFIER EXPRESSION
// identifier_expr = identifier |
//                   identifier '(' (binary_expr ',')* ')';
std::unique_ptr<AST::Expression> Parser::parse_identifier_expression(FILE* f) {
	std::vector<std::pair<std::string, IdentifierPart>> parts;

	while (1) {
		std::string identifier = *std::get_if<std::string>(&currentTokenVal);
		get_next_token(f); // Trim the identifier
		if (currentToken == '.') {
			parts.push_back(std::pair(identifier, IdentifierPart::FieldContainer);
			get_next_token(f);
		} else if (currentToken == ':') {
			parts.push_back(std::pair(identifier, IdentifierPart::Namespace);
			get_next_token(f);
		} else break;
	}

	if (currentToken != '(') {
		return std::make_unique<AST::VariableRef>(identifier, fields);; // Make variable reference if not function call
	}
	/*if (currentToken == '.') {
		std::cout << "struct ref" << std::endl;
		return parseStructRef(std::move(v));
	}*/

	std::string fullFuncLookupName;
	fullFuncLookupName += fields.size() > 0 ? "" : identifier;

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

	std::cout << "full internal func name is " << fullFuncLookupName << std::endl;

	get_next_token(f); // Trim (
	std::vector<std::unique_ptr<AST::Expression>> args;
	
	if (currentToken != ')') while (1) {
		args.push_back(parse_binary_expression(f));

		if (currentToken == ')') break;
		if (currentToken != ',') throw UNEXPECTED_CHAR(currentToken, ", after argument");
		get_next_token(f); // Trim ,
	}
	
	get_next_token(f); // Trim )
	return std::make_unique<AST::FunctionCall>(fullFuncLookupName, std::move(args));
}