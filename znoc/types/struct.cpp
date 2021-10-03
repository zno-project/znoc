#include "struct.hpp"
#include "type.hpp"
#include "../parsing.hpp"
#include "../macros.hpp"
#include "../main.hpp"
#include <exception>
#include <vector>
#include <variant>
#include <llvm/IR/DerivedTypes.h>
#include "../mangling.hpp"
#include <iostream>
#include <fmt/format.h>

llvm::Type* AST::StructType::codegen(int template_instance) {
	if (generated.find(template_instance) == generated.end()) {
		std::vector<llvm::Type*> f;
		for (auto &fT : fieldTypes) {
			if (std::holds_alternative<AST::templated_field_type_t>(fT)) { // If it's a templated field
				if (template_instance == -1) throw std::runtime_error(fmt::format("Struct {} requires template type", name));
				auto fT_t = std::get<AST::templated_field_type_t>(fT);
				auto fT_tt = templatedTypes.at(template_instance).at(fT_t.typeIndex);
				f.push_back(fT_tt.codegen());
			} else {
				f.push_back(std::get<AST::type_usage_t>(fT).codegen());
			}
		}
		generated[template_instance] = llvm::StructType::create(*TheContext, llvm::ArrayRef<llvm::Type*>(f));
		generated[template_instance]->setName(fmt::format("{}_instance{}", name, template_instance));
	}
	return generated[template_instance];
}

std::shared_ptr<AST::Type> Parser::parse_struct_def(FILE* f) {
	get_next_token(f);
	if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "struct name after 'struct'");
	std::string name = *std::get_if<std::string>(&currentTokenVal);
	get_next_token(f);

	std::map<std::string, int> template_type_names;
	int i = 0;

	if (currentToken == '<') {
		get_next_token(f);
		if (currentToken != '>') while (1) {
			std::string template_type_name = *std::get_if<std::string>(&currentTokenVal);
			template_type_names[template_type_name] = i;
			i++;
			get_next_token(f);
			if (currentToken != ',') break;
			get_next_token(f); 
		}
		if (currentToken != '>') throw UNEXPECTED_CHAR(currentToken, "`>` to end template type list");
		get_next_token(f);
	}

	if (currentToken != '{') throw UNEXPECTED_CHAR(currentToken, "{ after struct name");
	get_next_token(f);

	std::vector<std::pair<std::string, AST::field_type_t>> fields;

	if (currentToken != '}') while (1) {
		if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "identifier for struct field");
		std::string fieldName = *std::get_if<std::string>(&currentTokenVal);
		get_next_token(f);
		if (currentToken != ':') throw UNEXPECTED_CHAR(currentToken, ": after struct field name");
		get_next_token(f);

		try {
			AST::type_usage_t fieldType = parse_type(f);
			fields.push_back(std::pair<std::string, AST::type_usage_t>(fieldName, std::move(fieldType)));
		} catch (std::runtime_error) {
			std::string t_name = *std::get_if<std::string>(&currentTokenVal);
			auto template_type_idx = template_type_names.at(t_name);
			AST::templated_field_type_t fieldType = {
				template_type_idx
			};
			fields.push_back(std::pair<std::string, AST::templated_field_type_t>(fieldName, std::move(fieldType)));
			get_next_token(f);
		}

		if (currentToken == '}') break;
		else if (currentToken != ',') throw UNEXPECTED_CHAR(currentToken, ", or } after struct field");
		get_next_token(f); // Trim ,
	}
	get_next_token(f); // Trim }

	std::vector<AST::field_type_t> fieldTypes;
	std::map<std::string, int> fieldIndices;

	for (size_t i = 0; i < fields.size(); i++) {
		fieldTypes.push_back(fields[i].second);
		fieldIndices[fields[i].first] = i;
	}

	auto s = std::make_shared<AST::StructType>(name, std::move(fieldTypes), std::move(fieldIndices), std::vector<std::vector<AST::type_usage_t>>());
	return s;
}