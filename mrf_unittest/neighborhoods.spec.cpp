#include "neighborhoods.h"
#include <vector>
#include <catch.hpp>

TEST_CASE("Neighborhoods")
{
    size_t rows = 10;
    size_t cols = 10;

    auto neighborsOf = [&] (const Pixel& pixel)  {
        std::vector<Pixel> neighbors;

        for(const auto& n : FourNeighbors(rows, cols, pixel))
            neighbors.push_back(n);

        return neighbors;
    };

    SECTION("Inside pixel has all four neighbors")
    {
        std::vector<Pixel> expected {Pixel(4,5), Pixel(5, 4), Pixel(6, 5), Pixel(5, 6)};
        REQUIRE(neighborsOf(Pixel(5,5)) == expected);
    }

    SECTION("Top left corner has bottm and right neighbors")
    {
        std::vector<Pixel> expected { Pixel(1,0), Pixel(0, 1) };
        REQUIRE(neighborsOf(Pixel(0,0)) == expected);
    }

    SECTION("Bottom right corner has top and left neighbors")
    {
        std::vector<Pixel> expected { Pixel(8,9), Pixel(9,8) };
        REQUIRE(neighborsOf(Pixel(9,9)) == expected);
    }
}
