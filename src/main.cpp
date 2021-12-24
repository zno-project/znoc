#include <string>
#include <iostream>
#include <exception>
#include <vector>
#include <optional>
#include <variant>
#include <filesystem>
#include <map>
#include <deque>
#include <fmt/core.h>
#include <typeinfo>

#include "main.hpp"
#include "parsing.hpp"
#include "constructions/reference.hpp"
#include "constructions/function_def.hpp"
#include "constructions/namespace.hpp"
#include "types/type.hpp"

#include "llvm_module.hpp"

std::vector<std::shared_ptr<AST::Namespace>> NamespaceStack;

int main(int argc, char *argv[]) {
	std::string out_name = "output.o";
	if (argc == 3) out_name = argv[2];
	if (argc < 2) throw std::runtime_error("No input file specified");

	auto path = std::filesystem::current_path().append(argv[1]);

	make_llvm_module(path.string());

	push_new_scope();
	NamespaceStack = { std::make_shared<AST::Namespace>("_G") };
	AST::init_builtin_types();

	parse_file(path, NamespaceStack[0]);

	init_objcode_producer();

	NamespaceStack[0]->codegen();

	produce_objcode(out_name);
	return 0;
}
