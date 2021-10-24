#include "builtins.hpp"
#include "type.hpp"
#include "../main.hpp"
#include "../constructions/namespace.hpp"

AST::TypeInstance AST::get_fundamental_type(std::string name) {
	return GlobalNamespace->get_namespace_by_name("_fundamentals")->get_type_by_name(name);
}