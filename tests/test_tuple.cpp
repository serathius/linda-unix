#include "gtest/gtest.h"
#include "../include/tuple.h"

TEST (TupleTest, test_tuple_get_int)
{
    int value = 2131232132;
    Tuple<int> tuple(value);
    EXPECT_EQ(value, tuple.get<int>(0));
}

TEST (TupleTest, test_tuple_get_float)
{
    float value = 1.312321321313123213;
    Tuple<float> tuple(value);
    EXPECT_EQ(value, tuple.get<float>(0));
}

TEST (TupleTest, test_tuple_get_string)
{
    std::string value = "abrakadabra";
    Tuple<std::string> tuple(value);
    EXPECT_EQ(value, tuple.get<std::string>(0));
}

static const int integer = 2132131231;
static const float floating = 31312.321312;
static const std::string str = "hokus pokus";

TEST (TupleTest, test_tuple_get_all0)
{
    Tuple<int, float, std::string> tuple(integer, floating, str);
    EXPECT_EQ(integer, tuple.get<int>(0));
    EXPECT_EQ(floating, tuple.get<float>(1));
    EXPECT_EQ(str, tuple.get<std::string>(2));
}

TEST (TupleTest, test_tuple_get_all1)
{
    Tuple<int, std::string, float> tuple(integer, str, floating);
    EXPECT_EQ(integer, tuple.get<int>(0));
    EXPECT_EQ(floating, tuple.get<float>(2));
    EXPECT_EQ(str, tuple.get<std::string>(1));
}

TEST (TupleTest, test_tuple_get_all2)
{
    Tuple<float, int, std::string> tuple(floating, integer, str);
    EXPECT_EQ(integer, tuple.get<int>(1));
    EXPECT_EQ(floating, tuple.get<float>(0));
    EXPECT_EQ(str, tuple.get<std::string>(2));
}

TEST (TupleTest, test_tuple_get_all3)
{
    Tuple<std::string, int, float> tuple(str, integer, floating);
    EXPECT_EQ(integer, tuple.get<int>(1));
    EXPECT_EQ(floating, tuple.get<float>(2));
    EXPECT_EQ(str, tuple.get<std::string>(0));
}

TEST (TupleTest, test_tuple_get_all4)
{
    Tuple<float, std::string, int> tuple(floating, str, integer);
    EXPECT_EQ(integer, tuple.get<int>(2));
    EXPECT_EQ(floating, tuple.get<float>(0));
    EXPECT_EQ(str, tuple.get<std::string>(1));
}

TEST (TupleTest, test_tuple_get_all5)
{
    Tuple<std::string, float, int> tuple(str, floating, integer);
    EXPECT_EQ(integer, tuple.get<int>(2));
    EXPECT_EQ(floating, tuple.get<float>(1));
    EXPECT_EQ(str, tuple.get<std::string>(0));
}
