#include "pratt_parser.hpp"
#include "expression.hpp"
#include "../parsing.hpp"
#include <memory>
#include <stdio.h>

char *operator_to_string[] = {".", "::", "+", "-", "*", "/", "%", "&", "|", "~", "^", "&&", "||", "!", "<<", ">>", "==", "!=", "<", ">", "<=", ">=", "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=", "is", "[]"};

