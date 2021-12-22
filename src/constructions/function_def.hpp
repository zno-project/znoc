#ifndef _FUNCTION_DEF_H
#define _FUNCTION_DEF_H

#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include "../memory/variable.hpp"
#include "../memory/memory_location.hpp"
#include "../types/type.hpp"
#include "../attributes.hpp"
#include "codeblock.hpp"

#include <optional>

llvm::AllocaInst* create_entry_block_alloca(llvm::Function *in_func, std::string name, llvm::Type* t);

namespace AST {
	class Function: public MemoryLoc {
		public:
		struct arg_data_t {
			std::shared_ptr<AST::Variable> var_ref;
			std::unique_ptr<AST::Expression> default_value;
		};
		std::vector<arg_data_t> args;

		private:
		AST::TypeInstance returnType;
		std::unique_ptr<AST::CodeBlock> body;
		bool is_member_func;
		std::optional<std::string> varargs_name;
		attributes_t attributes;
		std::shared_ptr<AST::Variable> varargs_var;

		static std::vector<AST::TypeInstance> get_args(std::vector<arg_data_t>& args) {
			std::vector<AST::TypeInstance> ret;
			for (auto& arg : args) {
				ret.push_back(arg.var_ref->underlying_type);
			}
			return ret;
		}

		public:
		Function(std::string name, std::vector<arg_data_t> args, AST::TypeInstance returnType, attributes_t attributes, std::unique_ptr<AST::CodeBlock> body, bool is_member_func, std::optional<std::string> varargs_name, std::shared_ptr<AST::Variable> varargs_var): MemoryLoc(returnType.get_function_returning(Function::get_args(args), varargs_name.has_value()), name), args(std::move(args)), returnType(returnType), body(std::move(body)), is_member_func(is_member_func), varargs_name(varargs_name), attributes(attributes), varargs_var(varargs_var) {}
		void codegen_prototype();
		llvm::Value* codegen(llvm::IRBuilder<> *builder) override;
		AST::TypeInstance getRetType() { return returnType; } ;
		virtual std::string get_name() { return name; }
	};
}

namespace Parser {
	std::shared_ptr<AST::Function> parse_function(zno_ifile& f, attributes_t attributes, std::optional<AST::TypeInstance> self_type = std::optional<AST::TypeInstance>());
}

#endif
