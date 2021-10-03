#include "type.hpp"
#include "builtins.hpp"
#include "../parsing.hpp"
#include "../macros.hpp"
#include "../main.hpp"
#include "../constructions/namespace.hpp"
#include <map>
#include <string>
#include <fmt/format.h>

//std::map<std::string, std::shared_ptr<AST::Type>> named_types;

void AST::init_builtin_types() {
	*GlobalNamespace << std::make_shared<AST::llvm_builtin_type_int<1>>();
	*GlobalNamespace << std::make_shared<AST::llvm_builtin_type_int<8>>();
	*GlobalNamespace << std::make_shared<AST::llvm_builtin_type_int<16>>();
	*GlobalNamespace << std::make_shared<AST::llvm_builtin_type_int<32>>();
	*GlobalNamespace << std::make_shared<AST::llvm_builtin_type_int<64>>();
	*GlobalNamespace << std::make_shared<AST::llvm_builtin_type_int<128>>();
	*GlobalNamespace << std::make_shared<AST::llvm_builtin_type_half>();
	*GlobalNamespace << std::make_shared<AST::llvm_builtin_type_float>();
	*GlobalNamespace << std::make_shared<AST::llvm_builtin_type_double>();
	*GlobalNamespace << std::make_shared<AST::llvm_builtin_type_fp128>();
	*GlobalNamespace << std::make_shared<AST::llvm_builtin_type_void>();
	*GlobalNamespace << std::make_shared<AST::llvm_builtin_type_ptr>();
}

AST::type_usage_t Parser::parse_type(FILE* f) {
	if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "name of type");
	std::string name = *std::get_if<std::string>(&currentTokenVal);
	/*if (currentToken != '<') return std::make_unique<AST::Type>(name, std::vector<std::unique_ptr<AST::Type>>());
	get_next_token();
	
	std::vector<std::shared_ptr<AST::Type>> typeArgs;

	while (1) {
		typeArgs.push_back(parse_type());
		if (currentToken == '>') break;
		if (currentToken != ',') throw UNEXPECTED_CHAR(currentToken, ", or > after type argument");
		get_next_token(); // Trim ,
	}
	get_next_token(); // Trim >
	//return std::make_unique<AST::Type>(name, std::move(typeArgs));*/

	auto t = AST::get_type_by_name(name);
	get_next_token(f);

	if (currentToken == '<') {
		std::vector<AST::type_usage_t> template_types;
		get_next_token(f);
		if (currentToken != '>') while (1) {
			template_types.push_back(Parser::parse_type(f));
			if (currentToken != ',') break;
			get_next_token(f); 
		}
		if (currentToken != '>') throw UNEXPECTED_CHAR(currentToken, "`>` to end template type list");
		get_next_token(f);
		t.type->templatedTypes.push_back(template_types);
		t.template_instance = t.type->templatedTypes.size()-1; // The template instance of the type used - TODO: For a templated type, actually look up which instance it is, or create a new one
	}

	return t;
}

#include <iostream>

std::pair<std::unique_ptr<AST::Expression>, AST::type_usage_t> AST::Type::get_field(std::string field_name, int template_instance) {
	auto idx = fieldIndices.at(field_name);
	auto field_type = fieldTypes[idx];
	AST::type_usage_t ret;

	if (std::holds_alternative<AST::templated_field_type_t>(field_type)) { // If it's a templated field
		auto idx = std::get<AST::templated_field_type_t>(field_type).typeIndex; // Get the index in the template instance list
		ret = templatedTypes[template_instance][idx]; // And get the type for the current template instance
	} else {
		ret = std::get<AST::type_usage_t>(field_type); // If not templates, just get the raw type
	}

	return std::pair(std::make_unique<AST::GEP>(idx, ret),  ret);
}

// Returns a list of indices into nested structs, from field names and template instance
/*std::vector<int> AST::Type::get_field_idxs(std::vector<std::string> fieldNames, int template_instance) {
	std::vector<int> fieldIdx;
	auto currentT = this;
	for (auto &field : fieldNames) { // For each field in the list of fields
		auto idx = currentT->fieldIndices.at(field); // Get the index by field name
		fieldIdx.push_back(idx); // Append to return val

		auto newT = currentT->fieldTypes[idx]; // Get the type for that field
		if (std::holds_alternative<AST::templated_field_type_t>(newT)) { // If it's a templated field
			auto idx = std::get<AST::templated_field_type_t>(newT).typeIndex; // Get the index in the template instance list
			currentT = templatedTypes[template_instance][idx].type.get(); // And get the type for the current template instance
		} else {
			currentT = std::get<AST::type_usage_t>(newT).type.get(); // If not templates, just get the raw type
		}
	}
	return fieldIdx;
}*/

/*const std::string& AST::Type::get_part_mangle_name(int template_instance) {
	std::string templateTypes;
	std::vector<std::pair<ManglePart, std::string>> v = {std::pair(ManglePart::Type, name)};
	if (template_instance > -1) {
		for (auto templateType : templatedTypes.at(template_instance)) {
			templateTypes += templateType.type->get_part_mangle_name(templateType.template_instance);
		}
		v.push_back(std::pair(ManglePart::Typeargs, templateTypes));
	}
	return _mangle_name(v);
}*/