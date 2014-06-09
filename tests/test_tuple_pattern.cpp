#include "gtest/gtest.h"
#include "../include/tuple_pattern.h"

//INT

TEST (TuplePatternTest, test_tuplepattern_match_int_any)
{
    TuplePattern<int> pattern("*");
    Tuple<int> tuple(1);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_eq0)
{
    TuplePattern<int> pattern("123");
    Tuple<int> tuple(123);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_eq1)
{
    TuplePattern<int> pattern(123);
    Tuple<int> tuple(123);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_eq2)
{
    TuplePattern<int> pattern("12");
    Tuple<int> tuple(123);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_eq3)
{
    TuplePattern<int> pattern(12);
    Tuple<int> tuple(123);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_lt0)
{
    TuplePattern<int> pattern("<100");
    Tuple<int> tuple(99);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_lt1)
{
    TuplePattern<int> pattern("<100");
    Tuple<int> tuple(123);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_lt2)
{
    TuplePattern<int> pattern("<100");
    Tuple<int> tuple(100);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_lte0)
{
    TuplePattern<int> pattern("<=100");
    Tuple<int> tuple(99);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_lte1)
{
    TuplePattern<int> pattern("<=100");
    Tuple<int> tuple(100);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_lte2)
{
    TuplePattern<int> pattern("<=100");
    Tuple<int> tuple(123);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_gt0)
{
    TuplePattern<int> pattern(">100");
    Tuple<int> tuple(99);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_gt1)
{
    TuplePattern<int> pattern(">100");
    Tuple<int> tuple(123);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_gt2)
{
    TuplePattern<int> pattern(">100");
    Tuple<int> tuple(100);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_gte0)
{
    TuplePattern<int> pattern(">=100");
    Tuple<int> tuple(99);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_gte1)
{
    TuplePattern<int> pattern(">=100");
    Tuple<int> tuple(100);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_int_gte2)
{
    TuplePattern<int> pattern(">=100");
    Tuple<int> tuple(123);
    EXPECT_EQ(true, pattern.match(tuple));
}

// FLOAT -----------------------------------------------------------------------

TEST (TuplePatternTest, test_tuplepattern_match_float_any)
{
    TuplePattern<float> pattern("*");
    Tuple<float> tuple(1);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_eq0)
{
    TuplePattern<float> pattern("1.00001");
    Tuple<float> tuple(1.00001);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_eq1)
{
    TuplePattern<float> pattern(1.00001);
    Tuple<float> tuple(1.00001);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_eq2)
{
    TuplePattern<float> pattern("12");
    Tuple<float> tuple(1.00001);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_eq3)
{
    TuplePattern<float> pattern(12.0321);
    Tuple<float> tuple(1.00001);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_lt0)
{
    TuplePattern<float> pattern("<1");
    Tuple<float> tuple(0.999);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_lt1)
{
    TuplePattern<float> pattern("<1");
    Tuple<float> tuple(1.00001);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_lt2)
{
    TuplePattern<float> pattern("<1");
    Tuple<float> tuple(1);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_lte0)
{
    TuplePattern<float> pattern("<=1");
    Tuple<float> tuple(0.9999);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_lte1)
{
    TuplePattern<float> pattern("<=1");
    Tuple<float> tuple(1);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_lte2)
{
    TuplePattern<float> pattern("<=1");
    Tuple<float> tuple(1.00001);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_gt0)
{
    TuplePattern<float> pattern(">1");
    Tuple<float> tuple(0.999);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_gt1)
{
    TuplePattern<float> pattern(">1");
    Tuple<float> tuple(1.00001);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_gt2)
{
    TuplePattern<float> pattern(">1");
    Tuple<float> tuple(1);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_gte0)
{
    TuplePattern<float> pattern(">=1");
    Tuple<float> tuple(0.999);
    EXPECT_EQ(false, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_gte1)
{
    TuplePattern<float> pattern(">=1");
    Tuple<float> tuple(1);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_float_gte2)
{
    TuplePattern<float> pattern(">=1");
    Tuple<float> tuple(1.00001);
    EXPECT_EQ(true, pattern.match(tuple));
}

// STRING-----------------------------------------------------------------------

TEST (TuplePatternTest, test_tuplepattern_match_string_any)
{
    TuplePattern<std::string> pattern("*");
    Tuple<std::string> tuple("abrakadabra");
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_string_eq0)
{
    TuplePattern<std::string> pattern("abrakadabra");
    Tuple<std::string> tuple("abrakadabra");
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_string_eq1)
{
    TuplePattern<std::string> pattern("hokus pokus");
    Tuple<std::string> tuple("abrakadabra");
    EXPECT_EQ(false, pattern.match(tuple));
}

//COMBINATIONS------------------------------------------------------------------

TEST (TuplePatternTest, test_tuplepattern_match_combination0)
{
    TuplePattern<int, float, std::string> pattern("*", "*", "*");
    Tuple<int, float, std::string> tuple(123, 1.001, "abrakarabra");
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_combination1)
{
    TuplePattern<int, std::string, float> pattern("*", "*", "*");
    Tuple<int, std::string, float> tuple(123, "abrakarabra", 1.001);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_combination2)
{
    TuplePattern<float, int, std::string> pattern("*", "*", "*");
    Tuple<float, int, std::string> tuple(1.0313, 123, "abrakarabra");
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_combination3)
{
    TuplePattern<std::string, int, float> pattern("*", "*", "*");
    Tuple<std::string, int, float> tuple("abrakarabra", 123, 1.001);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_combination4)
{
    TuplePattern<float, std::string, int> pattern("*", "*", "*");
    Tuple<float, std::string, int> tuple(1.001, "abrakarabra", 123);
    EXPECT_EQ(true, pattern.match(tuple));
}

TEST (TuplePatternTest, test_tuplepattern_match_combination5)
{
    TuplePattern<std::string, float, int> pattern("*", "*", "*");
    Tuple<std::string, float, int> tuple("abrakarabra", 1.001, 123);
    EXPECT_EQ(true, pattern.match(tuple));
}
