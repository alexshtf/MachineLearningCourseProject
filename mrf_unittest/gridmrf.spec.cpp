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

        WHEN("Dimensiona are queried")
        {
            THEN("Correct dimensions are returned")
            {
                REQUIRE(mrf.rows() == rows);
                REQUIRE(mrf.cols() == cols);
                REQUIRE(mrf.labels() == labels);
                REQUIRE(mrf.neighborsCapacity() == neighborsCapacity);
            }
        }

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

        WHEN("Pairwise potential is stored")
        {
            Pixel pixel1(2, 1);
            Pixel pixel2(2, 2);
            mrf.setPairwise(pixel1, pixel2, 17.0);

            THEN("Retrieved potential for different labels is the stored one")
            {
                REQUIRE(mrf.getPairwise(pixel1, pixel2, 0, 0) == 0);
                REQUIRE(mrf.getPairwise(pixel2, pixel1, 0, 0) == 0);

                REQUIRE(mrf.getPairwise(pixel1, pixel2, 0, 1) == 17.0);
                REQUIRE(mrf.getPairwise(pixel2, pixel1, 0, 1) == 17.0);
            }
        }
    }
}
