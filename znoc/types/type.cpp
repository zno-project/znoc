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
	auto fundamentals = std::make_unique<AST::Namespace>("_fundamentals");
	*fundamentals << std::make_shared<AST::fundamental_int<1>>();
	*fundamentals << std::make_shared<AST::fundamental_int<8>>();
	*fundamentals << std::make_shared<AST::fundamental_int<16>>();
	*fundamentals << std::make_shared<AST::fundamental_int<32>>();
	*fundamentals << std::make_shared<AST::fundamental_int<64>>();
	*fundamentals << std::make_shared<AST::fundamental_int<128>>();
	*fundamentals << std::make_shared<AST::fundamental_half>();
	*fundamentals << std::make_shared<AST::fundamental_float>();
	*fundamentals << std::make_shared<AST::fundamental_double>();
	*fundamentals << std::make_shared<AST::fundamental_fp128>();
	*fundamentals << std::make_shared<AST::fundamental_void>();
	*fundamentals << std::make_shared<AST::fundamental_ptr>();

	*GlobalNamespace << std::move(fundamentals);
}

AST::TypeInstance Parser::parse_type(FILE* f) {
	if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "name of type");
	auto parsed_namespace_data = Parser::parse_namespace(f);

	std::string type_name = parsed_namespace_data.next_identifier;
	auto type_base = parsed_namespace_data.parsed_namespace->get_type_by_name(type_name);

	if (currentToken == '<') {
		std::vector<AST::TypeInstance> template_types;
		get_next_token(f);
		if (currentToken != '>') while (1) {
			template_types.push_back(Parser::parse_type(f));
			if (currentToken != ',') break;
			get_next_token(f); 
		}
		if (currentToken != '>') throw UNEXPECTED_CHAR(currentToken, "`>` to end template type list");
		get_next_token(f);
		//t.type->templatedTypes.push_back(template_types);
		//t.template_instance = t.type->templatedTypes.size()-1; // The template instance of the type used - TODO: For a templated type, actually look up which instance it is, or create a new one
	}

	return AST::TypeInstance {
		.base_type = std::move(type_base),
		.template_instance_id = -1
	};
}

std::shared_ptr<AST::Function> AST::TypeBase::get_function_by_name(std::string func_name, int template_instance_id) {
	try {
		auto f = functions.at(func_name);
		return f;
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Cannot find function {}:{}(...)", name, func_name));
	}
}

AST::FieldInfo AST::TypeBase::get_field_info_by_name(std::string field_name, int template_instance_id) {
	try {
		auto f_idx = fields_by_name.at(field_name);
		return get_field_info_by_index(f_idx, template_instance_id);
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Cannot find field {}:{}", name, field_name));
	}
}

AST::FieldInfo AST::TypeBase::get_field_info_by_index(int idx, int template_instance_id) {
	auto f = AST::FieldInfo {
		.type = std::get<AST::TypeInstance>(fields_by_index.at(idx)),
		.index = idx
	};
	return f;
}

std::shared_ptr<AST::TypeBase> Parser::parse_aggregate_type_definition(FILE* f) {
	get_next_token(f);
	if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "class/struct name after `class`/`struct`");
	std::string name = *std::get_if<std::string>(&currentTokenVal);
	get_next_token(f);

	if (currentToken != '{') throw UNEXPECTED_CHAR(currentToken, "{ after class/struct name");
	get_next_token(f);

	std::vector<std::pair<std::string, AST::TypeInstance>> fields;
	std::map<std::string, std::shared_ptr<AST::Function>> functions;

	if (currentToken != '}') while (1) {
		if (currentToken == tok_func) {
			auto func = Parser::parse_function(f);
			functions[func->get_name()] = func;
			//globalFuncDeclarations.push_back(std::pair<std::unique_ptr<AST::Function>, bool>(std::move(f), false));
		} else {
			if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "identifier for class/struct field");
			std::string fieldName = *std::get_if<std::string>(&currentTokenVal);
			get_next_token(f);
			if (currentToken != ':') throw UNEXPECTED_CHAR(currentToken, ": after class/struct field name");
			get_next_token(f);

			AST::TypeInstance fieldType = parse_type(f);

			fields.push_back(std::pair<std::string, AST::TypeInstance>(fieldName, std::move(fieldType)));
		}

		if (currentToken == '}') break;
		else if (currentToken != ',') throw UNEXPECTED_CHAR(currentToken, ", or } after class field/function");
		get_next_token(f); // Trim ,
	}
	get_next_token(f); // Trim }

	std::vector<AST::field_type_t> fields_by_index;
	std::map<std::string, size_t> fields_by_name;

	for (size_t i = 0; i < fields.size(); i++) {
		fields_by_index.push_back(fields[i].second);
		fields_by_name[fields[i].first] = i;
	}

	auto s = std::make_shared<AST::TypeBase>(name, std::move(fields_by_name), std::move(fields_by_index), std::move(functions));
	return s;
}

llvm::Type* AST::TypeBase::codegen(int template_instance) {
	if (!generated) {
		for (auto &func : functions) {
			func.second->codegen_prototype();
		}
		for (auto &func : functions) {
			func.second->codegen();
		}

		std::vector<llvm::Type*> f;
		for (auto &fT : fields_by_index) {
			f.push_back(std::get<AST::TypeInstance>(fT).codegen());
		}
		auto gen = llvm::StructType::create(*TheContext, llvm::ArrayRef<llvm::Type*>(f));
		gen->setName(name);
		generated = gen;
	}
	return generated;
};

/*#include <iostream>

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
}*/

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