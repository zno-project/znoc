#ifndef _MACROS_H
#define _MACROS_H

#include <fmt/format.h>
#include "location.hpp"

#define UNEXPECTED_CHAR(unexpected, expected) \
	std::runtime_error(fmt::format("{}:{}: Unexpected {}({}). Expected {} at {}:{}", __FILE__, __LINE__, (char)unexpected, (int)unexpected, expected, LexLoc.Line, LexLoc.Col));

#endif
