#ifndef _ZNO_TYPEDEF_H
#define _ZNO_TYPEDEF_H

#include "../parsing.hpp"
#include "namespace.hpp"
#include <stdio.h>

namespace Parser {
	void parse_typedef(FILE *f, AST::Namespace &n);
}

#endif
