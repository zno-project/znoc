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
	auto fundamentals = std::make_shared<AST::Namespace>("_fundamentals");
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_int<1>>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_int<8>>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_int<16>>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_int<32>>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_int<64>>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_int<128>>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_half>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_float>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_double>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_fp128>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_void>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_ptr>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_function>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
	*fundamentals << AST::TypeInstance {
		.base_type = std::make_shared<AST::fundamental_namespace_ref>(),
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};

	*GlobalNamespace << std::move(fundamentals);
}

void AST::TypeInstance::get_or_create_template(std::vector<AST::TypeInstance> template_types) {
	auto expected_num_template_args = base_type->get_generic_type_interfaces().size();
	auto found_num_template_args = template_types.size();

	if (is_templateable() && found_num_template_args != 0) { // Allow not 'generic' version of type eg. in typedefs
		if (expected_num_template_args != found_num_template_args) throw std::runtime_error(fmt::format("Expected {} typeargs for type {}. Found {}.", expected_num_template_args, base_type->get_name(), found_num_template_args));
		if (template_instance_id.has_value()) throw std::runtime_error(fmt::format("Cannot template already templated type {}", base_type->get_name()));
		template_instance_id = base_type->add_generic_instance(template_types);
	} else {
		// Cannot template untemplatable type
		if (found_num_template_args > 0) throw std::runtime_error(fmt::format("Cannot template type {}", base_type->get_name()));
	}
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

	/*auto expected_num_template_args = type_base.base_type->get_generic_type_interfaces().size();
	auto found_num_template_args = template_types.size();

	if (type_base.is_templateable() && found_num_template_args != 0) { // Allow not 'generic' version of type eg. in typedefs
		if (expected_num_template_args != found_num_template_args) throw std::runtime_error(fmt::format("Expected {} typeargs for type {}. Found {}.", expected_num_template_args, type_name, found_num_template_args));
		if (type_base.template_instance_id.has_value()) throw std::runtime_error(fmt::format("Cannot template already templated type {}", type_name));
		type_base.template_instance_id = type_base.base_type->add_generic_instance(template_types);
	} else {
		// Cannot template untemplatable type
		if (found_num_template_args > 0) throw std::runtime_error(fmt::format("Cannot template type {}", type_name));
	}*/
	type_base.get_or_create_template(template_types);

	while (1) {
		IF_TOK_ELSE('[', {
			bool is_float;
			size_t array_len;
			std::tie(array_len, is_float) = EXPECT_NUMERIC_LITERAL("array length");
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
	auto f_idx = fields_by_name.find(field_name);
	auto c_idx = constants.find(field_name);

	if (f_idx != fields_by_name.end()) {
		return get_field_info_by_index(f_idx->second, template_instance_id);
	} else if (c_idx != constants.end()) {
		return FieldInfoConst {
			.type = c_idx->second.type,
			.linkage_name = fmt::format("{}.{}", name, field_name)
		};
	} else {
		throw std::runtime_error(fmt::format("Cannot find field {}::{}", name, field_name));
	}
}

AST::FieldInfo AST::TypeBase::get_field_info_by_index(size_t idx, size_t template_instance_id) {
	auto f = AST::FieldInfoField {
		.type = std::holds_alternative<AST::TypeInstance>(fields_by_index.at(idx)) ? std::get<AST::TypeInstance>(fields_by_index.at(idx)) : generic_types.at(template_instance_id).at(std::get<AST::GenericInstance>(fields_by_index.at(idx)).generic_type_index),
		.index = idx
	};
	return f;
}

AST::TypeInstance finalise_aggregate_type(std::string name, std::vector<std::pair<std::string, AST::field_type_t>> fields, std::vector<AST::Interface> template_type_interfaces, std::map<std::string, AST::Constant> constants) {
	std::vector<AST::field_type_t> fields_by_index;
	std::map<std::string, size_t> fields_by_name;

	for (size_t i = 0; i < fields.size(); i++) {
		fields_by_index.push_back(fields[i].second);
		fields_by_name[fields[i].first] = i;
	}

	auto s = std::make_shared<AST::TypeBase>(name, std::move(fields_by_name), std::move(fields_by_index), std::move(template_type_interfaces), std::move(constants));

	return AST::TypeInstance {
		.base_type = s,
		.template_instance_id = std::nullopt,
		.array_lengths = std::vector<size_t>()
	};
}

#include "../constructions/construction_parse.hpp"

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
	std::map<std::string, AST::Constant> constants;

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
			IF_TOK_ELSE(tok_const, {
				std::string const_name = EXPECT_IDENTIFIER("after `const`");
				
				std::optional<AST::TypeInstance> type;

				OPTIONAL(':', {
					type = parse_type(f);
				});

				EXPECT('=', " or type after const field name");

				std::unique_ptr<AST::Expression> val = parse_numeric_literal(f);
				if (!type) type = val->getType();

				AST::Constant constant;
				constant.type = type.value();
				constant.val = std::move(val);

				constants[const_name] = std::move(constant);
			}, {
				if (!all_fields_defined) {
					// If first member function, prevent additional fields being created, plus finalise the type
					all_fields_defined = true;
					ret = finalise_aggregate_type(name, std::move(fields), std::move(template_type_interfaces), std::move(constants));
				}
				ret.base_type->add_func(Parser::parse_function(f, ret));
			});
		});
	}, "struct fields and functions");

	if (!all_fields_defined) ret = finalise_aggregate_type(name, std::move(fields), std::move(template_type_interfaces), std::move(constants));

	return ret;
}

llvm::Type* AST::TypeBase::codegen(size_t template_instance) {
	if (!generated[template_instance]) {
		for (auto &c: constants) {
			std::cout << "gen const with linkage name " << fmt::format("{}.{}", name, c.first) << std::endl;
			new llvm::GlobalVariable(*TheModule, c.second.type.codegen(), true, llvm::GlobalValue::PrivateLinkage, c.second.val->codegen_const(), fmt::format("{}.{}", name, c.first));
		}

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
			func.second->codegen(nullptr);
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

AST::TypeInstance AST::TypeInstance::get_function_returning() {
	auto t = AST::get_fundamental_type("function");
	t.template_instance_id = t.base_type->add_generic_instance({*this});
	return t;
}

AST::TypeInstance AST::TypeInstance::get_array_elem_of() {
	auto t = *this;
	t.array_lengths.pop_back();
	return t;
}

#include "builtins.hpp"

AST::TypeInstance AST::TypeInstance::get_pointed_to() {
	if (is_array()) throw std::runtime_error("Can only deref a pointer - attempted to deref an array");
	auto p = std::dynamic_pointer_cast<AST::fundamental_ptr>(base_type);
	if (!p) throw std::runtime_error(fmt::format("Can only deref a pointer - attempted to deref {}", this->base_type->get_name()));
	return p->get_pointed_to(get_template_id());
}

AST::TypeInstance AST::TypeInstance::get_return_of_function() {
	auto p = std::dynamic_pointer_cast<AST::fundamental_function>(base_type);
	if (!p) throw std::runtime_error(fmt::format("Can only get ret type of function - attempted to get ret type of {}", this->base_type->get_name()));
	return p->get_return_of_function(get_template_id());
}

void AST::TypeBase::add_func(std::shared_ptr<AST::Function> f) {
	functions.insert({f->get_name(), f});
}

llvm::Type* AST::TypeInstance::codegen() {
	auto t = base_type->codegen(get_template_id());
	for (auto& len : array_lengths) {
		t = llvm::ArrayType::get(t, len);
	}
	return t;
}

bool AST::TypeInstance::is_templateable() {
	return base_type->get_generic_type_interfaces().size() != 0;
}

bool AST::TypeInstance::is_array() {
	return array_lengths.size() > 0;
}

bool AST::TypeInstance::operator==(const TypeInstance& cmp) const {
	if (cmp.array_lengths.size() != array_lengths.size()) return false;
	if (cmp.base_type != base_type) return false;
	if (cmp.template_instance_id != template_instance_id) return false;
	return cmp.array_lengths == array_lengths;
};

std::shared_ptr<AST::Function> AST::TypeInstance::get_function_by_name(std::string name) {
	return base_type->get_function_by_name(name, get_template_id());
}

AST::FieldInfo AST::TypeInstance::get_field_info_by_name(std::string name) {
	return base_type->get_field_info_by_name(name, get_template_id());
}

AST::FieldInfo AST::TypeInstance::get_field_info_by_index(size_t idx) {
	return base_type->get_field_info_by_index(idx, get_template_id());
}

size_t AST::TypeInstance::get_template_id() {
	if (is_templateable()) {
		if (!template_instance_id.has_value()) throw std::runtime_error(fmt::format("Type {} must be templated before use", base_type->get_name()));
		return template_instance_id.value();
	}
	return 0;
}
