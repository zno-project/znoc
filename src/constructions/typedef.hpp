#ifndef _ZNO_TYPEDEF_H
#define _ZNO_TYPEDEF_H

#include "../parsing.hpp"
#include "namespace.hpp"

namespace Parser {
	void parse_typedef(zno_ifile& f, AST::Namespace &n);
}

#endif
