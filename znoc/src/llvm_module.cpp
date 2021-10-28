#include "llvm_module.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/Host.h>
#include <llvm/Analysis/TargetLibraryInfo.h>

#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>
#include <llvm/Transforms/Scalar/ADCE.h>

#include <memory>

std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::Module> TheModule;
std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;

llvm::TargetMachine *targetMachine;

void make_llvm_module(std::string name) {
	// Module construction
	TheContext = std::make_unique<llvm::LLVMContext>();
	TheModule = std::make_unique<llvm::Module>(name, *TheContext);
	TheModule->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 3);
	TheModule->addModuleFlag(llvm::Module::Error, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);

	TheFPM = std::make_unique<llvm::legacy::FunctionPassManager>(TheModule.get());

	TheFPM->add(new llvm::TargetLibraryInfoWrapperPass());
	// Remove dead code
	TheFPM->add(llvm::createDeadCodeEliminationPass());
	// Promote allocas to registers.
	TheFPM->add(llvm::createPromoteMemoryToRegisterPass());
	// Do simple "peephole" optimizations and bit-twiddling optzns.
	TheFPM->add(llvm::createInstructionCombiningPass());
	// Reassociate expressions.
	TheFPM->add(llvm::createReassociatePass());
	// Eliminate Common SubExpressions.
	TheFPM->add(llvm::createGVNPass());
	// Simplify the control flow graph (deleting unreachable blocks, etc).
	TheFPM->add(llvm::createCFGSimplificationPass());

	TheFPM->doInitialization();
}

void init_objcode_producer() {
	llvm::InitializeAllTargetInfos();
	llvm::InitializeAllTargets();
	llvm::InitializeAllTargetMCs();
	llvm::InitializeAllAsmParsers();
	llvm::InitializeAllAsmPrinters();

	auto TargetTriple = llvm::sys::getDefaultTargetTriple();

	std::string Error;
	auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

	// Print an error and exit if we couldn't find the requested target.
	// This generally occurs if we've forgotten to initialise the
	// TargetRegistry or we have a bogus target triple.
	if (!Target) {
		llvm::errs() << Error;
		exit(1);
	}

	auto CPU = "generic";
	auto Features = "";

	llvm::TargetOptions opt;
	auto RM = llvm::Optional<llvm::Reloc::Model>();
	targetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

	TheModule->setDataLayout(targetMachine->createDataLayout());
	TheModule->setTargetTriple(TargetTriple);
}

void produce_objcode(std::string filename) {
	std::error_code EC;
	llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);

	if (EC) {
		llvm::errs() << "Could not open file: " << EC.message();
		exit(2);
	}

	llvm::legacy::PassManager pass;
	auto FileType = llvm::CGFT_ObjectFile;

	pass.add(llvm::createAlwaysInlinerLegacyPass());

	if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
		llvm::errs() << "TargetMachine can't emit a file of this type";
		exit(3);
	}

	pass.run(*TheModule);
	dest.flush();
}