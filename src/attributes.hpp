#ifndef _ATTRIBUTES_H
#define _ATTRIBUTES_H

#include "parsing.hpp"
#include <optional>

namespace Attributes {
	enum Attributes {
		AlwaysInline,
		Extern,
		END
	};
}

struct attributes_t {
	bool always_inline = false;
	bool extern_ = false;
	std::optional<std::string> mangle_name = std::nullopt;
};

namespace Parser {
	attributes_t parse_attributes(zno_ifile& f);
}

#endif
