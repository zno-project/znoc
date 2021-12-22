#ifndef _ZNO_STRUCT_INIT_H
#define _ZNO_STRUCT_INIT_H

#include "expression.hpp"
#include "../types/type.hpp"

namespace Parser {
	std::unique_ptr<AST::Expression> parse_struct_init(zno_ifile& f, AST::TypeInstance to_init);
}

#endif
