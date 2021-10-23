#ifndef _MAIN_H
#define _MAIN_H

#include <memory>

namespace AST { class Namespace; }
extern std::unique_ptr<AST::Namespace> GlobalNamespace;

#endif