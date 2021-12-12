#ifndef _ZNO_TYPE_BASE
#define _ZNO_TYPE_BASE

#include "../constructions/namespace.hpp"
#include <string>
#include <map>
#include <vector>
#include <llvm/IR/DerivedTypes.h>
#include <variant>
#include <optional>
#include <fmt/format.h>
#include <memory>

namespace AST {
	class TypeBase: public AST::Namespace {
		protected:
		std::map<std::string, size_t> fields_by_name;
		std::vector<AST::field_type_t> fields_by_index;
		std::vector<std::vector<AST::TypeInstance>> generic_types;
		std::vector<Interface> generic_type_interfaces;
		std::map<std::string, std::shared_ptr<AST::Function>> functions;
		std::map<size_t, llvm::Type*> generated;

		public:
		TypeBase(std::string name): Namespace(name), fields_by_name(), fields_by_index(), generic_types(), generic_type_interfaces(), functions(), generated() {}
		TypeBase(std::string name, std::map<std::string, size_t> fields_by_name, std::vector<AST::field_type_t> fields_by_index, std::vector<Interface> generic_type_interfaces): Namespace(name), fields_by_name(std::move(fields_by_name)), fields_by_index(std::move(fields_by_index)), generic_types(), generic_type_interfaces(std::move(generic_type_interfaces)), functions(), generated() {}

		virtual llvm::Type* codegen(size_t template_instance);

		virtual ~TypeBase() = default;

		std::shared_ptr<AST::Function> get_function_by_name(std::string name, size_t template_instance_id);
		FieldInfo get_field_info_by_name(std::string name, size_t template_instance_id);
		FieldInfo get_field_info_by_index(size_t idx, size_t template_instance_id);
		size_t add_generic_instance(std::vector<AST::TypeInstance> types);
		std::vector<Interface>& get_generic_type_interfaces() {
			return generic_type_interfaces;
		}

		void add_func(std::shared_ptr<AST::Function> f);
	};
}

#endif