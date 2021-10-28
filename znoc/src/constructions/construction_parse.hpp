#ifndef _CONSTRUCTION_PARSE_H
#define _CONSTRUCTION_PARSE_H

#include <stdio.h>

namespace AST { class Expression; }

namespace Parser {
	std::unique_ptr<AST::Expression> parse_binary_expression(FILE* f);
	std::unique_ptr<AST::Expression> parse_code_block(FILE* f);
	std::unique_ptr<AST::Expression> parse_continue(FILE* f);
	std::unique_ptr<AST::Expression> parse_break(FILE* f);
	std::unique_ptr<AST::Expression> parse_if_def(FILE* f);
	std::unique_ptr<AST::Expression> parse_numeric_literal(FILE* f);
	std::unique_ptr<AST::Expression> parse_identifier_expression(FILE* f);
	std::unique_ptr<AST::Expression> parse_switch(FILE* f);
	std::unique_ptr<AST::Expression> parse_return(FILE* f);
	std::unique_ptr<AST::Expression> parse_unary_expression(FILE* f);
	std::unique_ptr<AST::Expression> parse_while(FILE* f);
	std::unique_ptr<AST::Expression> parse_variable_def(FILE* f);
	std::unique_ptr<AST::Expression> parse_fallthrough(FILE* f);
	std::unique_ptr<AST::Expression> parse_string(FILE* f);
}

#endif