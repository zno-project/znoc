#include "type.hpp"
#include "builtins.hpp"
#include "../parsing.hpp"
#include "../macros.hpp"
#include "../main.hpp"
#include "../llvm_module.hpp"
#include "../constructions/namespace.hpp"
#include <map>
#include <string>
#include <fmt/format.h>
#include <optional>
#include <algorithm>

//std::map<std::string, std::shared_ptr<AST::Type>> named_types;

void AST::init_builtin_types() {
	auto fundamentals = std::make_unique<AST::Namespace>("_fundamentals");
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_int<1>>(),
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_int<8>>(),
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_int<16>>(),
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_int<32>>(),
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_int<64>>(),
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_int<128>>(),
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_half>(),
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_float>(),
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_double>(),
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_fp128>(),
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_void>(),
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_ptr>(),
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};

	*GlobalNamespace << std::move(fundamentals);
}

AST::TypeInstance Parser::parse_type(FILE* f) {
	if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "name of type");
	auto parsed_namespace_data = Parser::parse_namespace(f);

	std::string type_name = parsed_namespace_data.next_identifier;
	auto type_base = parsed_namespace_data.parsed_namespace->get_type_by_name(type_name);

	std::vector<AST::TypeInstance> template_types;
	OPTIONAL_LIST('<', ',', '>', {
		template_types.push_back(Parser::parse_type(f));
	}, "template list");

	auto expected_num_template_args = type_base.base_type->get_generic_type_interfaces().size();
	auto found_num_template_args = template_types.size();

	if (type_base.is_templateable() && found_num_template_args != 0) { // Allow not 'generic' version of type eg. in typedefs
		if (expected_num_template_args != found_num_template_args) throw std::runtime_error(fmt::format("Expected {} typeargs for type {}. Found {}.", expected_num_template_args, type_name, found_num_template_args));
		if (type_base.template_instance_id.has_value()) throw std::runtime_error(fmt::format("Cannot template already templated type {}", type_name));
		type_base.template_instance_id = type_base.base_type->add_generic_instance(template_types);
	} else {
		// Cannot template untemplatable type
		if (found_num_template_args > 0) throw std::runtime_error(fmt::format("Cannot template type {}", type_name));
	}

	while (1) {
		IF_TOK_ELSE('[', {
			bool is_float;
			size_t array_len;
			std::tie(array_len, is_float) = EXPECT_NUMBERIC_LITERAL("array length");
			if (is_float) throw std::runtime_error("Array lengths cannot float. They are required to sink.");
			EXPECT(']', "after array length");
			type_base.array_lengths.push_back(array_len);
		}, { break; });
	}

	return type_base;
}

std::shared_ptr<AST::Function> AST::TypeBase::get_function_by_name(std::string func_name, size_t template_instance_id) {
	try {
		auto f = functions.at(func_name);
		return f;
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Cannot find function {}:{}(...)", name, func_name));
	}
}

AST::FieldInfo AST::TypeBase::get_field_info_by_name(std::string field_name, size_t template_instance_id) {
	try {
		auto f_idx = fields_by_name.at(field_name);
		return get_field_info_by_index(f_idx, template_instance_id);
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Cannot find field {}:{}", name, field_name));
	}
}

AST::FieldInfo AST::TypeBase::get_field_info_by_index(size_t idx, size_t template_instance_id) {
	auto f = AST::FieldInfo {
		.type = std::holds_alternative<AST::TypeInstance>(fields_by_index.at(idx)) ? std::get<AST::TypeInstance>(fields_by_index.at(idx)) : generic_types.at(template_instance_id).at(std::get<AST::GenericInstance>(fields_by_index.at(idx)).generic_type_index),
		.index = idx
	};
	return f;
}

AST::TypeInstance finalise_aggregate_type(std::string name, std::vector<std::pair<std::string, AST::field_type_t>> fields, std::vector<AST::Interface> template_type_interfaces) {
	std::vector<AST::field_type_t> fields_by_index;
	std::map<std::string, size_t> fields_by_name;

	for (size_t i = 0; i < fields.size(); i++) {
		fields_by_index.push_back(fields[i].second);
		fields_by_name[fields[i].first] = i;
	}

	auto s = std::make_shared<AST::TypeBase>(name, std::move(fields_by_name), std::move(fields_by_index), std::move(template_type_interfaces));

	return AST::TypeInstance {
		.base_type = s,
		.template_instance_id = std::optional<size_t>(),
		.array_lengths = std::vector<size_t>()
	};
}

AST::TypeInstance Parser::parse_aggregate_type_definition(FILE* f) {
	EXPECT(tok_struct, "to begin struct definition");

	std::string name = EXPECT_IDENTIFIER("for name of struct");//std::get<std::string>(currentTokenVal);

	std::map<std::string, size_t> template_type_names;
	std::vector<AST::Interface> template_type_interfaces;

	if (currentToken == '<') {
		get_next_token(f);
		if (currentToken != '>') while (1) {
			std::string template_type_name = std::get<std::string>(currentTokenVal);
			template_type_interfaces.push_back(AST::Interface {});
			template_type_names[template_type_name] = template_type_interfaces.size() - 1;
			get_next_token(f);
			if (currentToken != ',') break;
			get_next_token(f); 
		}
		if (currentToken != '>') throw UNEXPECTED_CHAR(currentToken, "`>` to end template type list");
		get_next_token(f);
	}

	std::vector<std::pair<std::string, AST::field_type_t>> fields;

	AST::TypeInstance ret;
	bool all_fields_defined = false;

	LIST('{', ',', '}', {
		IF_TOK_ELSE_IDENTIFIER(fieldName, {
			if (all_fields_defined) throw std::runtime_error("Cannot declare field after member function");
			
			EXPECT(':', "after struct field name");

			AST::field_type_t fieldType;

			try {
				std::string t_name = std::get<std::string>(currentTokenVal);
				auto template_type_idx = template_type_names.at(t_name);
				fieldType = AST::GenericInstance {
					.generic_type_index = template_type_idx
				};
				get_next_token(f);
			} catch (std::out_of_range) {
				fieldType = parse_type(f);
			}

			fields.push_back(std::pair<std::string, AST::field_type_t>(fieldName, std::move(fieldType)));
		}, {
			if (!all_fields_defined) {
				// If first member function, prevent additional fields being created, plus finalise the type
				all_fields_defined = true;
				ret = finalise_aggregate_type(name, std::move(fields), std::move(template_type_interfaces));
			}
			ret.base_type->add_func(Parser::parse_function(f, ret));
		});
	}, "struct fields and functions");

	if (!all_fields_defined) ret = finalise_aggregate_type(name, std::move(fields), std::move(template_type_interfaces));

	return ret;
}

llvm::Type* AST::TypeBase::codegen(size_t template_instance) {
	if (!generated[template_instance]) {
		std::vector<llvm::Type*> f;
		for (auto &fT : fields_by_index) {
			if (std::holds_alternative<AST::TypeInstance>(fT)) {
				f.push_back(std::get<AST::TypeInstance>(fT).codegen());
			} else {
				auto field_idx = std::get<AST::GenericInstance>(fT).generic_type_index;
				f.push_back(generic_types.at(template_instance).at(field_idx).codegen());
			}
		}
		auto gen = llvm::StructType::create(*TheContext, llvm::ArrayRef<llvm::Type*>(f));
		gen->setName(fmt::format("{}_instance{}", name, template_instance));
		generated[template_instance] = gen;

		for (auto &func : functions) {
			func.second->codegen_prototype();
		}
		for (auto &func : functions) {
			func.second->codegen();
		}
	}
	return generated[template_instance];
};

size_t AST::TypeBase::add_generic_instance(std::vector<AST::TypeInstance> types) {
	auto res = std::find(generic_types.begin(), generic_types.end(), types);
	if (res == generic_types.end()) {
		generic_types.push_back(std::move(types));
		return generic_types.size() - 1;
	} else {
		return res - generic_types.begin();
	}
}

AST::TypeInstance AST::TypeInstance::get_pointer_to() {
	auto t = AST::get_fundamental_type("ptr");
	t.template_instance_id = t.base_type->add_generic_instance({*this});
	return t;
}

#include "builtins.hpp"

AST::TypeInstance AST::TypeInstance::get_pointed_to() {
	if (is_array()) throw std::runtime_error("Can only deref a pointer - attempted to deref an array");
	auto p = std::dynamic_pointer_cast<AST::fundamental_ptr>(base_type);
	if (!p) throw std::runtime_error(fmt::format("Can only deref a pointer - attempted to deref {}", this->base_type->get_name()));
	return p->get_pointed_to(get_template_id());
}

void AST::TypeBase::add_func(std::shared_ptr<AST::Function> f) {
	functions.insert({f->get_name(), f});
}