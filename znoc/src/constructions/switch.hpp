#ifndef _SWITCH_H
#define _SWITCH_H

#include "expression.hpp"

#include <vector>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include "codeblock.hpp"
#include "../types/builtins.hpp"
#include "construction_parse.hpp"

namespace AST {
	class SwitchDef: public Expression {
		public:
		typedef struct {
			std::vector<llvm::ConstantInt*> num;
		} switch_case_metadata_t;
		
		private:
		std::unique_ptr<AST::Expression> switching_expr;
		std::vector<std::pair<switch_case_metadata_t, std::unique_ptr<AST::CodeBlock>>> body;
		std::unique_ptr<AST::CodeBlock> default_body;

		public:
		SwitchDef(std::unique_ptr<AST::Expression> switching_expr, std::vector<std::pair<switch_case_metadata_t, std::unique_ptr<AST::CodeBlock>>> body, std::unique_ptr<AST::CodeBlock> default_body): Expression(AST::get_fundamental_type("void")), switching_expr(std::move(switching_expr)), body(std::move(body)), default_body(std::move(default_body)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string _name = "");
	};
}

namespace Parser {
	std::pair<AST::SwitchDef::switch_case_metadata_t, std::unique_ptr<AST::CodeBlock>> parse_switch_case(FILE* f);
}

#endif