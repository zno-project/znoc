#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Constants.h>
#include <llvm/ADT/APInt.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>
#include "KaleidoscopeJIT.h"
#include <vector>
using namespace llvm;
using namespace llvm::orc;

raw_ostream &output = outs();
static std::unique_ptr<LLVMContext> TheContext;
#ifdef JIT
static std::unique_ptr<KaleidoscopeJIT> TheJIT;
#else
TargetMachine *targetMachine;
#endif
std::unique_ptr<Module> TheModule;
Type *va_list_type;
static ExitOnError ExitOnErr;

void makeLLVMModule() {
	// Module construction
	TheModule = std::make_unique<Module>("test", *TheContext);
	std::vector<Type*> va_list_elems;
	va_list_elems.push_back(PointerType::getUnqual(IntegerType::get(*TheContext, 8)));
	va_list_type = StructType::create(ArrayRef<Type*>(va_list_elems), "__va_list_type");
	//TheModule->getOrInsertGlobal("__va_list_type", va_list_type);
}

void addMainFunc(Module *mod) {
	FunctionCallee f = mod->getOrInsertFunction("main",
	/*ret type*/                                IntegerType::get(*TheContext, 32),
	/*args*/                                    IntegerType::get(*TheContext, 32));
	Function *mainF = cast<Function>(f.getCallee());
	mainF->setCallingConv(CallingConv::C);

	Function::arg_iterator args = mainF->arg_begin();
	Value *x = args++;
	x->setName("x");

	BasicBlock *block = BasicBlock::Create(*TheContext, "entry", mainF);
	IRBuilder<> builder(block);

	llvm::Function *personality = mod->getFunction("__gxx_personality_v0");
	llvm::LandingPadInst *caughtResult = builder.CreateLandingPad(IntegerType::get(*TheContext, 8),
                           1,
                           "landingPad");

	builder.CreateRet(x);
}

void addFMAFunc(Module *mod) {
	FunctionCallee f = mod->getOrInsertFunction("fma",
	/*ret type*/                                IntegerType::get(*TheContext, 32),
	/*args*/                                    IntegerType::get(*TheContext, 32),
	                                            IntegerType::get(*TheContext, 32),
	                                            IntegerType::get(*TheContext, 32));
	Function *fma = cast<Function>(f.getCallee());
	fma->setCallingConv(CallingConv::C);

	Function::arg_iterator args = fma->arg_begin();
	Value *x = args++;
	x->setName("x");
	Value *y = args++;
	y->setName("y");
	Value *z = args++;
	z->setName("z");

	BasicBlock *block = BasicBlock::Create(*TheContext, "entry", fma);
	IRBuilder<> builder(block);

	Value *tmp1 = builder.CreateBinOp(Instruction::Mul, x, y);
	Value *tmp2 = builder.CreateBinOp(Instruction::Add, tmp1, z);
	builder.CreateRet(tmp2);
}

void exceptionStuff(Module *mod) {
	llvm::Function *personality = mod->getFunction("__gxx_personality_v0");
}

void addGCDFunc(Module *mod) {
	/*
	unsigned gcd(unsigned x, unsigned y) {
	entry:
		if(x == y) {
	ret:
			return x;
		} else {
	args_not_equal:
			if(x < y) {
	x_less_y:
				return gcd(x, y - x);
			} else {
	x_not_less_y:
				return gcd(x - y, y);
			}
		}
	}
	*/
	FunctionCallee f = mod->getOrInsertFunction("gcd",
	                                            IntegerType::get(*TheContext, 32),
	                                            IntegerType::get(*TheContext, 32),
	                                            IntegerType::get(*TheContext, 32));
	Function *gcd = cast<Function>(f.getCallee());
	gcd->setCallingConv(CallingConv::C);

	Function::arg_iterator args = gcd->arg_begin();
	Value *x = args++;
	x->setName("x");
	Value *y = args++;
	y->setName("y");

	BasicBlock *entry = BasicBlock::Create(*TheContext, "entry", gcd);
	BasicBlock *ret = BasicBlock::Create(*TheContext, "return", gcd);
	BasicBlock *args_not_equal = BasicBlock::Create(*TheContext, "cond_false", gcd);
	BasicBlock *x_less_y = BasicBlock::Create(*TheContext, "x_less_y", gcd);
	BasicBlock *x_not_less_y = BasicBlock::Create(*TheContext, "x_not_less_y", gcd);

	// Test if x == y
	IRBuilder<> builder(entry);
	Value *x_equal_y = builder.CreateICmpEQ(x, y);
	builder.CreateCondBr(x_equal_y, ret, args_not_equal);

	// If yes, return x
	builder.SetInsertPoint(ret);
	builder.CreateRet(x);

	// If no, check if x<y
	builder.SetInsertPoint(args_not_equal);
	Value *x_less_y_val = builder.CreateICmpULT(x, y);
	builder.CreateCondBr(x_less_y_val, x_less_y, x_not_less_y);

	// If x<y
	{
		builder.SetInsertPoint(x_less_y);
		Value *y_sub_x = builder.CreateSub(y, x);

		std::vector<Value*> recursive_gcd_args;
		recursive_gcd_args.push_back(x);
		recursive_gcd_args.push_back(y_sub_x);
		Value *recursive_gcd_ret = builder.CreateCall(gcd, recursive_gcd_args);

		builder.CreateRet(recursive_gcd_ret);
	}

	// If x≥y
	{
		builder.SetInsertPoint(x_not_less_y);
		Value *x_sub_y = builder.CreateSub(x, y);

		std::vector<Value*> recursive_gcd_args;
		recursive_gcd_args.push_back(x);
		recursive_gcd_args.push_back(x_sub_y);
		Value *recursive_gcd_ret = builder.CreateCall(gcd, recursive_gcd_args);

		builder.CreateRet(recursive_gcd_ret);
	}
}

void addVariadicFunc(Module *mod) {
	std::vector<Type*> params;
	params.push_back(IntegerType::get(*TheContext, 32));
	params.push_back(IntegerType::get(*TheContext, 64));
	params.push_back(IntegerType::get(*TheContext, 1));

	FunctionType *type = FunctionType::get(IntegerType::get(*TheContext, 32), ArrayRef<Type*>(params), true);
	FunctionCallee f = mod->getOrInsertFunction("variadic",
	                                            type);

	Function *var = cast<Function>(f.getCallee());
	var->setCallingConv(CallingConv::C);

	Function::arg_iterator args = var->arg_begin();
	Value *x = args++;
	x->setName("x");
	Value *y = args++;
	y->setName("y");
	Value *b = args++;
	b->setName("b");

	BasicBlock *entry = BasicBlock::Create(*TheContext, "entry", var);
	IRBuilder<> builder(entry);

	Value *va_list_inst = builder.CreateAlloca(va_list_type, nullptr, "vari_args_list");

	std::vector<Value*> vaStartParams;
	vaStartParams.push_back(va_list_inst);

	Function *va_start = Intrinsic::getDeclaration(TheModule.get(), Intrinsic::vastart);
	Function *va_end = Intrinsic::getDeclaration(TheModule.get(), Intrinsic::vaend);
	
	builder.CreateCall(va_start, vaStartParams);
	Value *z = builder.CreateVAArg(va_list_inst, IntegerType::get(*TheContext, 32), "firstVariArg");
	Value *z2 = builder.CreateVAArg(va_list_inst, IntegerType::get(*TheContext, 32), "secondVariArg");
	builder.CreateCall(va_end, vaStartParams);

	builder.CreateRet(z2);
}

void initJIT() {
#ifdef JIT
	InitializeNativeTarget();
	InitializeNativeTargetAsmPrinter();
	InitializeNativeTargetAsmParser();

	TheJIT = ExitOnErr(KaleidoscopeJIT::Create());
	TheModule->setDataLayout(TheJIT->getDataLayout());
#endif
}

void runJIT() {
#ifdef JIT
	auto TSM = ThreadSafeModule(std::move(TheModule), std::move(TheContext));
	auto H = TheJIT->addModule(std::move(TSM));
	auto ExprSymbol = TheJIT->lookup("variadic").get();

	int32_t (*FP)(int32_t, int64_t, bool, ...) = (int32_t (*)(int32_t, int64_t, bool, ...))(intptr_t)ExprSymbol.getAddress();
	
	printf("variadic(1,2,true,3,4) evaluated to %d\n", FP(1,2,true,3,4));
#endif
}

void initObj() {
#ifndef JIT
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
#endif
}

void runObj() {
#ifndef JIT
	auto Filename = "output.o";
	std::error_code EC;
	raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

	if (EC) {
		errs() << "Could not open file: " << EC.message();
		exit(2);
	}

	legacy::PassManager pass;
	auto FileType = CGFT_ObjectFile;

	if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
		errs() << "TargetMachine can't emit a file of this type";
		exit(3);
	}

	pass.run(*TheModule);
	dest.flush();
#endif
}

int main() {
	TheContext = std::make_unique<LLVMContext>();
	makeLLVMModule();
#ifdef JIT
	initJIT();
#else
	initObj();
#endif

	//addFMAFunc(TheModule.get());
	//addGCDFunc(TheModule.get());
	addMainFunc(TheModule.get());
	//addVariadicFunc(TheModule.get());

	verifyModule(*TheModule, &output);

	legacy::PassManager PM;
	PM.add(createPrintModulePass(output));
	PM.run(*TheModule);

#ifdef JIT
	runJIT();
#else
	runObj();
#endif

	return 0;
}