#include "namespace.hpp"

#include <fmt/format.h>

AST::type_usage_t AST::Namespace::get_type_by_name(std::string name) {
	try {
		return {
			named_types.at(name), // type
			-1                    // index into template list - `-1` since not from templated type
		};
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Could not find type {}", name));
	}
}

std::shared_ptr<AST::Function> AST::Namespace::get_function_by_name(std::string name) {
	try {
		auto f = named_functions.at(name);
		return f;
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Cannot find function {}", name));
	}
}

AST::Namespace* AST::Namespace::get_namespace_by_name(std::string name) {
	try {
		return namespaces.at(name).get();
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Cannot find namespace {}", name));
	}
}