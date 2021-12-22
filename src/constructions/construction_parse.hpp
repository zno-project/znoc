#ifndef _CONSTRUCTION_PARSE_H
#define _CONSTRUCTION_PARSE_H

#include "../parsing.hpp"

namespace AST { class Expression; }

namespace Parser {
	std::unique_ptr<AST::Expression> parse_code_block(zno_ifile& f);
	std::unique_ptr<AST::Expression> parse_continue(zno_ifile& f);
	std::unique_ptr<AST::Expression> parse_break(zno_ifile& f);
	std::unique_ptr<AST::Expression> parse_if_def(zno_ifile& f);
	std::unique_ptr<AST::Expression> parse_numeric_literal(zno_ifile& f);
	std::unique_ptr<AST::Expression> parse_identifier_expression(zno_ifile& f);
	std::unique_ptr<AST::Expression> parse_switch(zno_ifile& f);
	std::unique_ptr<AST::Expression> parse_return(zno_ifile& f);
	std::unique_ptr<AST::Expression> parse_while(zno_ifile& f);
	std::unique_ptr<AST::Expression> parse_variable_def(zno_ifile& f);
	std::unique_ptr<AST::Expression> parse_fallthrough(zno_ifile& f);
	std::unique_ptr<AST::Expression> parse_string(zno_ifile& f);
	std::unique_ptr<AST::Expression> parse_pratt_expression(zno_ifile& f, unsigned int min_bp = 0);
}

#endif
