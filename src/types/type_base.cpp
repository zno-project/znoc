#include "type_base.hpp"
#include "../llvm_module.hpp"

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
	//auto c_idx = constants.find(field_name);

	if (f_idx != fields_by_name.end()) {
		return get_field_info_by_index(f_idx->second, template_instance_id);
	}/* else if (c_idx != constants.end()) {
		return FieldInfoConst {
			.type = c_idx->second.type,
			.linkage_name = fmt::format("{}.{}", name, field_name)
		};
	}*/ else {
		throw std::runtime_error(fmt::format("Cannot find field {}.{}", name, field_name));
	}
}

AST::FieldInfo AST::TypeBase::get_field_info_by_index(size_t idx, size_t template_instance_id) {
	auto f = AST::FieldInfoField {
		.type = std::holds_alternative<AST::TypeInstance>(fields_by_index.at(idx)) ? std::get<AST::TypeInstance>(fields_by_index.at(idx)) : generic_types.at(template_instance_id).at(std::get<AST::GenericInstance>(fields_by_index.at(idx)).generic_type_index),
		.index = idx
	};
	return f;
}


llvm::Type* AST::TypeBase::codegen(size_t template_instance) {
	if (!generated[template_instance]) {
		/*for (auto &c: constants) {
			std::cout << "gen const with linkage name " << fmt::format("{}.{}", name, c.first) << std::endl;
			new llvm::GlobalVariable(*TheModule, c.second.type.codegen(), true, llvm::GlobalValue::PrivateLinkage, c.second.val->codegen_const(), fmt::format("{}.{}", name, c.first));
		}*/

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
}

size_t AST::TypeBase::add_generic_instance(std::vector<AST::TypeInstance> types) {
	auto res = std::find(generic_types.begin(), generic_types.end(), types);
	if (res == generic_types.end()) {
		generic_types.push_back(std::move(types));
		return generic_types.size() - 1;
	} else {
		return res - generic_types.begin();
	}
}

void AST::TypeBase::add_func(std::shared_ptr<AST::Function> f) {
	functions.insert({f->get_name(), f});
}
