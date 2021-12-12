#ifndef _MANGLING_H
#define _MANGLING_H

#include <string>
#include <vector>

enum class ManglePart {
	Function,
	Type,
	ClosureFunc,
	ClosureStruct,
	Namespace,
	Typeargs
};

std::string full_mangle_name(int partCount, ...);
std::string full_mangle_name(std::vector<std::pair<ManglePart, std::string>> parts);
std::string _mangle_name(std::vector<std::pair<ManglePart, std::string>> parts);

#endif
