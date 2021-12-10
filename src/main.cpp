#include <stdio.h>
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

std::shared_ptr<AST::Namespace> GlobalNamespace;

int main(int argc, char *argv[]) {
	std::string out_name = "output.o";
	if (argc == 3) out_name = argv[2];
	if (argc < 2) throw std::runtime_error("No input file specified");

	auto path = std::filesystem::current_path().append(argv[1]);
	//std::cout << "path: " << path.string() << std::endl;

	make_llvm_module(path.string());

	push_new_scope();
	GlobalNamespace = std::make_shared<AST::Namespace>("_G");
	AST::init_builtin_types();

	parse_file(path, GlobalNamespace);

	init_objcode_producer();

	GlobalNamespace->codegen();

	produce_objcode(out_name);
	return 0;
}
