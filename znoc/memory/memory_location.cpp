#include "memory_location.hpp"
#include <map>
#include <deque>
#include <string>

std::deque<std::map<std::string, std::shared_ptr<AST::MemoryLoc>>> stack_allocations;