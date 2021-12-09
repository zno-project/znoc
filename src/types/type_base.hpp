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
		//std::string name;
		std::map<size_t, llvm::Type*> generated;
		std::map<std::string, size_t> fields_by_name;
		std::vector<AST::field_type_t> fields_by_index;
		std::vector<std::vector<AST::TypeInstance>> generic_types;
		std::vector<Interface> generic_type_interfaces;
		std::map<std::string, std::shared_ptr<AST::Function>> functions;
		std::map<std::string, Constant> constants;

		public:
		TypeBase(std::string name): /*name(std::move(name)),*/ fields_by_name(), fields_by_index(), functions(), generic_types(), generated(), generic_type_interfaces(), constants(), Namespace(name) {}
		//TypeBase(std::string name, std::map<std::string, std::map<std::string, size_t> fields_by_name): name(std::move(name)), fields(std::move(fields)), functions() {}
		TypeBase(std::string name, std::map<std::string, size_t> fields_by_name, std::vector<AST::field_type_t> fields_by_index, std::vector<Interface> generic_type_interfaces, std::map<std::string, Constant> constants): /*name(std::move(name)),*/ fields_by_name(std::move(fields_by_name)), generic_types(), fields_by_index(std::move(fields_by_index)), functions(), generic_type_interfaces(std::move(generic_type_interfaces)), generated(), constants(std::move(constants)), Namespace(name) {}

		virtual llvm::Type* codegen(size_t template_instance);

		virtual std::string get_name() { return name; }
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