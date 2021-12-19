#ifndef _ATTRIBUTES_H
#define _ATTRIBUTES_H

#include <bitset>
#include "parsing.hpp"

namespace Attributes {
	enum Attributes {
		AlwaysInline,
		Extern,
		END
	};
}

typedef std::bitset<Attributes::END> attributes_t;

namespace Parser {
	attributes_t parse_attributes(zno_ifile& f);
}

#endif
