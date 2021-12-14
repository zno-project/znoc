#ifndef _ATTRIBUTES_H
#define _ATTRIBUTES_H

#include <bitset>
#include "parsing.hpp"

enum class Attributes {
	AlwaysInline,
	NoMangle,
	END
};

typedef std::bitset<(unsigned long)Attributes::END> attributes_t;
extern attributes_t currentAttributes;

namespace Parser {
	void parse_attributes(zno_ifile& f);
	void clear_attributes();
}

#endif
