#ifndef _MAIN_H
#define _MAIN_H

#include <memory>

namespace AST { class Namespace; }
extern std::shared_ptr<AST::Namespace> GlobalNamespace;
extern std::shared_ptr<AST::Namespace> CurrentNamespace;

#endif
