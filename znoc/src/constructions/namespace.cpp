#include "namespace.hpp"
#include "../types/type.hpp"
#include "../parsing.hpp"
#include "../main.hpp"
#include "../macros.hpp"

#include <fmt/format.h>
#include <iostream>

std::shared_ptr<AST::TypeBase> AST::Namespace::get_type_by_name(std::string type_name) {
	try {
		return named_types.at(type_name);
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Could not find type {}:{}", name, type_name));
	}
}

std::shared_ptr<AST::Function> AST::Namespace::get_function_by_name(std::string func_name) {
	try {
		auto f = named_functions.at(func_name);
		return f;
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Cannot find function {}:{}(...)", name, func_name));
	}
}

AST::Namespace* AST::Namespace::get_namespace_by_name(std::string namespace_name) {
	try {
		return &*namespaces.at(namespace_name);
	} catch (std::out_of_range) {
		throw std::runtime_error(fmt::format("Cannot find namespace {}:{}", name, namespace_name));
	}
}

Parser::NamespaceParseReturn Parser::parse_namespace(FILE* f) {
	auto n = &*GlobalNamespace;
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
		.parsed_namespace = n,
		.next_identifier = std::move(identifier),
		.next_token = currentToken
	};
}