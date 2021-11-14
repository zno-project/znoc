#include "gtest/gtest.h"

#define private public
#define protected public

#include "framework.hpp"

#include "main.hpp"
#include "memory/memory_location.hpp"
#include "types/type.hpp"
#include "types/builtins.hpp"
#include "constructions/namespace.hpp"
#include "constructions/numeric_literal.hpp"
#include "constructions/construction_parse.hpp"
#include "constructions/pratt_parser.hpp"

#include <stdio.h>
#include <iostream>
#include <cfloat>

TEST(PrattParser, BasicAddition) {
	char test_data[] = "45 + 6";
	auto c = new CompilerMain(test_data);
	auto r = Parser::parse_pratt_expression(c->f);

	EXPECT_EQ(r->print(), "(45 + 6)");

	delete c;
}

TEST(PrattParser, AdditionAssociation) {
	char test_data[] = "45 + 6 + 5 + 3";

	auto c = new CompilerMain(test_data);
	auto r = Parser::parse_pratt_expression(c->f);

	EXPECT_EQ(r->print(), "(((45 + 6) + 5) + 3)");

	delete c;
}

TEST(PrattParser, Prefix) {
	char test_data[] = "+3 - 5";

	auto c = new CompilerMain(test_data);
	auto r = Parser::parse_pratt_expression(c->f);

	EXPECT_EQ(r->print(), "((+ 3) - 5)");

	delete c;
}

TEST(PrattParser, Duplicativity) {
	char test_data[] = "1 < 2 < 3";

	auto c = new CompilerMain(test_data);
	EXPECT_ANY_THROW(Parser::parse_pratt_expression(c->f));

	delete c;
}

TEST(PrattParser, Brackets) {
	char test_data[] = "1 + (2 + 3)";

	auto c = new CompilerMain(test_data);
	auto r = Parser::parse_pratt_expression(c->f);

	EXPECT_EQ(r->print(), "(1 + (2 + 3))");

	delete c;
}

TEST(PrattParser, ManyBrackets) {
	char test_data[] = "((((5))))";

	auto c = new CompilerMain(test_data);
	auto r = Parser::parse_pratt_expression(c->f);

	EXPECT_EQ(r->print(), "5");

	delete c;
}

TEST(PrattParser, MultiCharOperators) {
	char test_data[] = "1 <<= 2";

	auto c = new CompilerMain(test_data);
	auto r = Parser::parse_pratt_expression(c->f);

	EXPECT_EQ(r->print(), "(1 <<= 2)");

	delete c;
}

TEST(PrattParser, UnmatchedBrackets) {
	char test_data[] = "1 + (2 + 3";

	auto c = new CompilerMain(test_data);
	EXPECT_ANY_THROW(Parser::parse_pratt_expression(c->f));

	delete c;
}

TEST(PrattParser, Precedence) {
	char test_data[] = "1 + 2 * 3";

	auto c = new CompilerMain(test_data);
	auto r = Parser::parse_pratt_expression(c->f);

	EXPECT_EQ(r->print(), "(1 + (2 * 3))");

	delete c;
}

TEST(PrattParser, RightAssociation) {
	char test_data[] = "1 = 2 = 3";

	auto c = new CompilerMain(test_data);
	auto r = Parser::parse_pratt_expression(c->f);

	EXPECT_EQ(r->print(), "(1 = (2 = 3))");

	delete c;
}

TEST(PrattParser, Subscript) {
	char test_data[] = "1[2]";

	auto c = new CompilerMain(test_data);
	auto r = Parser::parse_pratt_expression(c->f);

	EXPECT_EQ(r->print(), "(1 [ 2)");

	delete c;
}

TEST(PrattParser, SubscriptMultiDimensional) {
	char test_data[] = "1[2][3]";

	auto c = new CompilerMain(test_data);
	auto r = Parser::parse_pratt_expression(c->f);

	EXPECT_EQ(r->print(), "((1 [ 2) [ 3)");

	delete c;
}

TEST(PrattParser, SubscriptUnmatchedBrackets) {
	char test_data[] = "1[2";

	auto c = new CompilerMain(test_data);
	EXPECT_ANY_THROW(Parser::parse_pratt_expression(c->f));

	delete c;
}
