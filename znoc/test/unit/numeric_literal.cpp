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

#include <stdio.h>
#include <cfloat>

testing::AssertionResult PredicateTypeEquality(const char* lhs_s, const char* rhs_s, AST::TypeInstance lhs, AST::TypeInstance rhs) {
	if (lhs == rhs) {
		return testing::AssertionSuccess();
	} else {
		return testing::AssertionFailure() << "Expected equality of these types:\n  " << lhs_s << "\n    (Type: " << lhs.base_type->get_name() << ")\n" << rhs_s << "\n    (Type: " << rhs.base_type->get_name() << ")\n";
	}
}

TEST(NumericLiteral, TestParsingOfBasicInt) {
	char *test_data = "45";
	auto c = new CompilerMain(test_data);
	auto r = Parser::parse_numeric_literal(c->f);

	auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
	ASSERT_NE(expr, nullptr);
	EXPECT_TRUE(expr->is_int_val);
	EXPECT_EQ(expr->value.i, 45);

	delete c;
}

TEST(NumericLiteral, TestParsingOfBasicFloat) {
	char *test_data = "45.6";
	auto c = new CompilerMain(test_data);
	auto r = Parser::parse_numeric_literal(c->f);

	auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
	ASSERT_NE(expr, nullptr);
	EXPECT_FALSE(expr->is_int_val);
	EXPECT_DOUBLE_EQ(expr->value.f, 45.6);

	delete c;
}

TEST(NumericLiteral, TestInvalidModifiers) {
	char *test_data = "45q8";
	auto c = new CompilerMain(test_data);

	EXPECT_ANY_THROW(Parser::parse_numeric_literal(c->f));

	delete c;
}

TEST(NumericLiteral, TestInvalidIntLengths) {
	char *test_data = "45u7";
	auto c = new CompilerMain(test_data);

	EXPECT_ANY_THROW(Parser::parse_numeric_literal(c->f));

	delete c;
}

TEST(NumericLiteral, TestInvalidFloatLengths) {
	char *test_data = "45f8";
	auto c = new CompilerMain(test_data);

	EXPECT_ANY_THROW(Parser::parse_numeric_literal(c->f));

	delete c;
}

TEST(NumericLiteral, TestLengthOfIntModifiers) {
	{
		char *test_data = "36u8";
		auto c = new CompilerMain(test_data);
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_PRED_FORMAT2(PredicateTypeEquality, expr->expressionType, AST::get_fundamental_type("i8"));

		delete c;
	}

	{
		char *test_data = "45u16";
		auto c = new CompilerMain(test_data);
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_PRED_FORMAT2(PredicateTypeEquality, expr->expressionType, AST::get_fundamental_type("i16"));

		delete c;
	}
}

TEST(NumericLiteral, TestLengthOfIntAutoLen) {
	{
		auto test_d = std::to_string((uint64_t)INT32_MAX);
		auto c = new CompilerMain(test_d.c_str());
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_PRED_FORMAT2(PredicateTypeEquality, expr->expressionType, AST::get_fundamental_type("i32"));

		delete c;
	}

	{
		auto test_d = std::to_string((uint64_t)INT32_MAX + 10);
		auto c = new CompilerMain(test_d.c_str());
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_PRED_FORMAT2(PredicateTypeEquality, expr->expressionType, AST::get_fundamental_type("i64"));

		delete c;
	}
}

TEST(NumericLiteral, TestLengthOfFloatModifiers) {
	{
		char *test_data = "36f16";
		auto c = new CompilerMain(test_data);
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_PRED_FORMAT2(PredicateTypeEquality, expr->expressionType, AST::get_fundamental_type("half"));

		delete c;
	}

	{
		char *test_data = "45f128";
		auto c = new CompilerMain(test_data);
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_PRED_FORMAT2(PredicateTypeEquality, expr->expressionType, AST::get_fundamental_type("fp128"));

		delete c;
	}
}

TEST(NumericLiteral, TestLengthOfFloatAutoLenIsDouble) {
	{
		auto test_d = std::to_string((double)FLT_MAX - 10);
		auto c = new CompilerMain(test_d.c_str());
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_PRED_FORMAT2(PredicateTypeEquality, expr->expressionType, AST::get_fundamental_type("double"));

		delete c;
	}

	{
		auto test_d = std::to_string((double)FLT_MAX);
		auto c = new CompilerMain(test_d.c_str());
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_PRED_FORMAT2(PredicateTypeEquality, expr->expressionType, AST::get_fundamental_type("double"));

		delete c;
	}

	{
		auto test_d = std::to_string((double)FLT_MAX + 10);
		auto c = new CompilerMain(test_d.c_str());
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_PRED_FORMAT2(PredicateTypeEquality, expr->expressionType, AST::get_fundamental_type("double"));

		delete c;
	}
}

TEST(NumericLiteral, TestParsingOfIntegerValueFloat) {
	{
		char *test_data = "3.0";
		auto c = new CompilerMain(test_data);
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_FALSE(expr->is_int_val);

		delete c;
	}
	{
		char *test_data = "6.0";
		auto c = new CompilerMain(test_data);
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_FALSE(expr->is_int_val);

		delete c;
	}
	{
		char *test_data = "1.0";
		auto c = new CompilerMain(test_data);
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_FALSE(expr->is_int_val);

		delete c;
	}
	{
		char *test_data = "0.0";
		auto c = new CompilerMain(test_data);
		auto r = Parser::parse_numeric_literal(c->f);

		auto expr = dynamic_cast<AST::NumericLiteral*>(r.release());
		ASSERT_NE(expr, nullptr);
		EXPECT_FALSE(expr->is_int_val);

		delete c;
	}
}