#ifndef _MAIN_H
#define _MAIN_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <vector>

namespace AST { class Namespace; }

extern std::unique_ptr<llvm::LLVMContext> TheContext;
extern std::unique_ptr<llvm::Module> TheModule;
extern std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;
extern std::unique_ptr<AST::Namespace> GlobalNamespace;

//extern std::vector<std::pair<std::unique_ptr<AST::Function>, bool>> globalFuncDeclarations;

#endif