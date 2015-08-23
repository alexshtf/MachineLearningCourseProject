#include <catch.hpp>
#include "gridmrf.h"

SCENARIO("Grid MRF stores potentials for unequal labels")
{
    GIVEN("A Grid MRF")
    {
        const size_t rows = 4;
        const size_t cols = 3;
        const size_t labels = 2;
        const size_t neighborsCapacity = 7;
        GridMRF mrf(rows, cols, labels, neighborsCapacity);

        WHEN("Unary potential is stored")
        {
            Pixel pixel(2, 1);
            mrf.setUnary(pixel, 1, 4.0);
            mrf.setUnary(pixel, 0, 3.0);

            THEN("Retrieved potential is the same")
            {
                REQUIRE(mrf.getUnary(pixel, 1) == 4.0);
                REQUIRE(mrf.getUnary(pixel, 0) == 3.0);
            }
        }
    }
}
