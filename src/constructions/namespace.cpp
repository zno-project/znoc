#include "namespace.hpp"
#include "../types/type.hpp"
#include "../parsing.hpp"
#include "../main.hpp"
#include "../macros.hpp"

#include <fmt/format.h>
#include <iostream>

AST::TypeInstance AST::Namespace::get_type_by_name(std::string type_name) {
	try {
		return named_types.at(type_name);
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Could not find type {}::{}", name, type_name));
	}
}

/*std::shared_ptr<AST::Function> AST::Namespace::get_function_by_name(std::string func_name) {
	try {
		auto f = global_variables.at(func_name);
		return f;
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Cannot find function {}::{}(...)", name, func_name));
	}
}*/

std::shared_ptr<AST::Namespace> AST::Namespace::get_namespace_by_name(std::string namespace_name) {
	try {
		return namespaces.at(namespace_name);
	} catch (std::out_of_range) {
	/*	throw std::runtime_error(fmt::format("Cannot find namespace {}::{}", name, namespace_name));
	}
}

std::shared_ptr<AST::MemoryLoc> AST::Namespace::get_global_var_by_name(std::string var_name) {
	try {
		return named_variables.at(var_name);
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Cannot find variable {}:{}", name, var_name));*/
		//std::cerr << fmt::format("Cannot find variable {}:{}", name, namespace_name) << std::endl;
		try {
			return std::dynamic_pointer_cast<AST::Namespace>(named_types.at(namespace_name).base_type);
		} catch (std::out_of_range) {}
		return nullptr;
	}
}

Parser::NamespaceParseReturn Parser::parse_namespace(FILE* f) {
	auto n = GlobalNamespace;
	std::string identifier;
	while (1) {
		identifier = std::get<std::string>(currentTokenVal);
		get_next_token(f);
		if (currentToken == ':') {
			get_next_token(f);
			if (currentToken != ':') throw UNEXPECTED_CHAR(currentToken, "`::` in namespace accessor");
			n = n->get_namespace_by_name(identifier);
			get_next_token(f);
		}
		else break;
	}

	return Parser::NamespaceParseReturn {
		.parsed_namespace = &*n,
		.next_identifier = std::move(identifier),
		.next_token = currentToken
	};
}

#include "../types/type_base.hpp"

AST::Namespace* AST::Namespace::operator <<(AST::TypeInstance t) {
	add_type_with_name(t, t.base_type->get_name());
	return this;
}

AST::Namespace* AST::Namespace::operator <<(std::shared_ptr<AST::MemoryLoc> f) {
	global_variables.insert({f->name, f});
	return this;
}

AST::Namespace* AST::Namespace::operator <<(std::shared_ptr<AST::Namespace> n) {
	//std::cout << "Creating new global namespace called `" << n->get_name() << "`" << std::endl;
	namespaces.insert({n->get_name(), std::move(n)});
	return this;
}
