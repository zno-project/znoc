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

#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>
#include <llvm/Transforms/Scalar/ADCE.h>
#include <llvm/IR/DIBuilder.h>

using namespace llvm;
raw_ostream &output = outs();

std::unique_ptr<LLVMContext> TheContext;
std::unique_ptr<Module> TheModule;
std::unique_ptr<legacy::FunctionPassManager> TheFPM;
std::unique_ptr<AST::Namespace> GlobalNamespace;

int level = 0;
TargetMachine *targetMachine;

void makeLLVMModule(std::string name) {
	// Module construction
	TheContext = std::make_unique<LLVMContext>();
	TheModule = std::make_unique<Module>(name, *TheContext);
	TheModule->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 3);
	TheModule->addModuleFlag(llvm::Module::Error, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);

	TheFPM = std::make_unique<legacy::FunctionPassManager>(TheModule.get());

	// Remove dead code
	TheFPM->add(createDeadCodeEliminationPass());
	// Promote allocas to registers.
	TheFPM->add(createPromoteMemoryToRegisterPass());
	// Do simple "peephole" optimizations and bit-twiddling optzns.
	TheFPM->add(createInstructionCombiningPass());
	// Reassociate expressions.
	TheFPM->add(createReassociatePass());
	// Eliminate Common SubExpressions.
	TheFPM->add(createGVNPass());
	// Simplify the control flow graph (deleting unreachable blocks, etc).
	TheFPM->add(createCFGSimplificationPass());

	TheFPM->doInitialization();

	//DBuilder = new DIBuilder(*TheModule);
	//TheCU = DBuilder->createCompileUnit(dwarf::DW_LANG_C, DBuilder->createFile(name, "."), "./a.out", 0, "", 0);
}

void initObj() {
	InitializeAllTargetInfos();
	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmParsers();
	InitializeAllAsmPrinters();

	auto TargetTriple = sys::getDefaultTargetTriple();

	std::string Error;
	auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

	// Print an error and exit if we couldn't find the requested target.
	// This generally occurs if we've forgotten to initialise the
	// TargetRegistry or we have a bogus target triple.
	if (!Target) {
		errs() << Error;
		exit(1);
	}

	auto CPU = "generic";
	auto Features = "";

	TargetOptions opt;
	auto RM = Optional<Reloc::Model>();
	targetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

	TheModule->setDataLayout(targetMachine->createDataLayout());
	TheModule->setTargetTriple(TargetTriple);
}

void runObj() {
	auto Filename = "output.o";
	std::error_code EC;
	raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

	if (EC) {
		errs() << "Could not open file: " << EC.message();
		exit(2);
	}

	/*llvm::ModuleAnalysisManager mam;

	llvm::FunctionPassManager fpm;
	fpm.addPass(llvm::ADCEPass());

	llvm::ModulePassManager mpm;
	mpm.addPass(createModuleToFunctionPassAdaptor(std::move(fpm)));
	mpm.run(*TheModule, mam);
	printf("done thing mpm thingy\n");*/
	legacy::PassManager pass;
	auto FileType = CGFT_ObjectFile;

	pass.add(createAlwaysInlinerLegacyPass());

	if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
		errs() << "TargetMachine can't emit a file of this type";
		exit(3);
	}

	pass.run(*TheModule);
	dest.flush();
}

//std::vector<std::pair<std::unique_ptr<AST::Function>, bool>> globalFuncDeclarations;

int main(int argc, char *argv[]) {
	if (argc != 2) throw std::runtime_error("No input file specified");

	auto path = std::filesystem::current_path().append(argv[1]);
	std::cout << "path: " << path.string() << std::endl;

	makeLLVMModule(path.string());

	stack_allocations.push_front(std::map<std::string, std::shared_ptr<AST::MemoryLoc>>());

	//std::vector<std::pair<std::unique_ptr<AST::VariableDef>, bool>> variableDeclarations;
	//std::vector<std::pair<std::unique_ptr<AST::Type>, bool>> typeDeclarations;

	GlobalNamespace = std::make_unique<AST::Namespace>("_G");
	AST::init_builtin_types();

	parse_file(path, GlobalNamespace);

	initObj();

	GlobalNamespace->codegen();

	TheModule->print(outs(), nullptr);
	std::error_code EC;
	llvm::raw_fd_ostream fd = raw_fd_ostream(StringRef("output_opt.ll"), EC);

	runObj();
	TheModule->print(fd, nullptr);
	return 0;
}