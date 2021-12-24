#ifndef _MAIN_H
#define _MAIN_H

#include <memory>
#include <vector>

namespace AST { class Namespace; }
extern std::vector<std::shared_ptr<AST::Namespace>> NamespaceStack;

#endif
