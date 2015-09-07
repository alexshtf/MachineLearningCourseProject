#include "util.h"
#include <catch.hpp>

double sqrtAbs(int x)
{
    return sqrt(abs(x));
}

TEST_CASE("finding maxima in range")
{
    std::vector<int> numbers = {6, -3, -100, -50};

    SECTION("max_and_argmax find both maximum and maximizer")
    {
        double max;
        int argmax;
        std::tie(max, argmax) = max_and_argmax(begin(numbers), end(numbers), sqrtAbs);

        REQUIRE(max == 10); // the maximum square root of absolute value is (sqrt(abs(-100)) = 10
        REQUIRE(argmax == -100); // this is the element in the array that achieves this maximum
    }

    SECTION("max_and_argmax works on whole ranges")
    {
        auto resultForIterators = max_and_argmax(begin(numbers), end(numbers), sqrtAbs);
        auto resultForRange = max_and_argmax(numbers, sqrtAbs);

        REQUIRE(resultForIterators == resultForRange);
    }

    SECTION("max_value is consistent with max_and_argmax")
    {
        auto resultForIterators = max_value(begin(numbers), end(numbers), sqrtAbs);
        auto resultForRange = max_value(numbers, sqrtAbs);

        REQUIRE(resultForIterators == 10);
        REQUIRE(resultForRange == 10);
    }
}
