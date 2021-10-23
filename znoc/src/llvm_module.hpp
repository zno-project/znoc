#ifndef _ZNO_LLVM_MODULE_H
#define _ZNO_LLVM_MODULE_H

#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>

extern std::unique_ptr<llvm::LLVMContext> TheContext;
extern std::unique_ptr<llvm::Module> TheModule;
extern std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;

void init_objcode_producer();
void produce_objcode(std::string filename);
void make_llvm_module(std::string name);

#endif