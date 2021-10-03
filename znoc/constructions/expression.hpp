#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/BasicBlock.h>

#include "../location.hpp"
#include "../types/type.hpp"

extern llvm::BasicBlock *mergeBB; // Merge block of current control flow codegen
extern llvm::BasicBlock *condBB; // Condition block of current control flow codegen

namespace AST {
	class Expression {
		SourceLocation Loc;
		AST::type_usage_t expressionType;

		public:
		Expression(AST::type_usage_t t, SourceLocation Loc = LexLoc) : Loc(Loc), expressionType(std::move(t)) {}
		virtual llvm::Value* codegen(llvm::IRBuilder<> *builder, std::string name = "") = 0;
		virtual ~Expression() = default;
		int getLine() const { return Loc.Line; }
		int getCol() const { return Loc.Col; }
		AST::type_usage_t getType() { return expressionType; }
	};
}

namespace Parser {
	std::unique_ptr<AST::Expression> parse_parentheses_expression(FILE* f);
	std::unique_ptr<AST::Expression> parse_non_semicolon_statement(FILE* f);
	std::unique_ptr<AST::Expression> parse_r_value(FILE* f);
	std::unique_ptr<AST::Expression> parse_semicolon_statement(FILE* f);
	std::unique_ptr<AST::Expression> parse_statement(FILE* f);
}

#endif