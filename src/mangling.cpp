#include "mangling.hpp"
#include <string>
#include <stdarg.h>
#include <iostream>
#include <exception>

std::string part_to_id(ManglePart p) {
	if (p == ManglePart::ClosureFunc) return "Cf";
	else if (p == ManglePart::ClosureStruct) return "Ct";
	else if (p == ManglePart::Function) return "F";
	else if (p == ManglePart::Namespace) return "N";
	else if (p == ManglePart::Type) return "T";
	else if (p == ManglePart::Typeargs) return "Y";
	throw std::runtime_error("HOW VERY DARE YOU");
}

std::string full_mangle_name(int partCount, ...) {
	va_list args;
	va_start(args, partCount);

	std::vector<std::pair<ManglePart, std::string>> parts;

	for (int i = 0; i < partCount; i++) {
		ManglePart t = va_arg(args, ManglePart);
		std::string n = std::string(va_arg(args, char*));
		parts.push_back(std::pair(t, n));
	}

	va_end(args);

	return full_mangle_name(parts);
}

std::string _mangle_name(std::vector<std::pair<ManglePart, std::string>> parts) {
	std::string out;
	std::string namespaces;
	std::string names;

	for (auto &part: parts) {
		ManglePart t = part.first;
		std::string n = part.second;

		names += std::to_string(n.length()) + n;
		namespaces.insert(0, part_to_id(t));
	}

	out += namespaces + names;
	return out;
}

std::string full_mangle_name(std::vector<std::pair<ManglePart, std::string>> parts) {
	return "_G" + _mangle_name(parts);
}
