#include "gridmrf.h"
#include <catch.hpp>
#include <algorithm>

namespace {
    template<typename TContainer>
    std::vector<typename TContainer::value_type> sorted(const TContainer& container)
    {
        std::vector<typename TContainer::value_type> vec(container.begin(), container.end());
        std::sort(begin(vec), end(vec));
        return vec;
    }
}

SCENARIO("Grid MRF stores a graph and potentials for unequal labels")
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

        WHEN("Two pairwise potentials are stored")
        {
            Pixel pixel1(2, 1);
            Pixel pixel2(2, 2);
            Pixel pixel3(3, 1);
            Pixel pixel4(3, 2);

            mrf.setPairwise(pixel1, pixel2, 17.0); // pixels 1 and 2 are connected, with pairwise potential of 17 for unequal labels
            mrf.setPairwise(pixel3, pixel1, 14.0); // pixels 1 and 3 are connected, with pairwise potential of 14 for unequal labels

            THEN("Retrieved potential for different labels is the stored one")
            {
                // same label queries
                REQUIRE(mrf.getPairwise(pixel1, pixel2, 0, 0) == 0);
                REQUIRE(mrf.getPairwise(pixel2, pixel1, 0, 0) == 0);
                REQUIRE(mrf.getPairwise(pixel1, pixel3, 0, 0) == 0);
                REQUIRE(mrf.getPairwise(pixel3, pixel1, 0, 0) == 0);

                // different label queries
                REQUIRE(mrf.getPairwise(pixel1, pixel2, 0, 1) == 17.0);
                REQUIRE(mrf.getPairwise(pixel2, pixel1, 0, 1) == 17.0);
                REQUIRE(mrf.getPairwise(pixel1, pixel3, 0, 1) == 14.0);
                REQUIRE(mrf.getPairwise(pixel3, pixel1, 0, 1) == 14.0);
            }


            THEN("Iteration over neighbors produces a correct list")
            {
                REQUIRE(sorted(mrf.neighbors(pixel1)) == std::vector<Pixel>({ pixel2, pixel3 }));
                REQUIRE(sorted(mrf.neighbors(pixel2)) == std::vector<Pixel>({ pixel1 }));
                REQUIRE(sorted(mrf.neighbors(pixel3)) == std::vector<Pixel>({ pixel1 }));
                REQUIRE(sorted(mrf.neighbors(pixel4)) == std::vector<Pixel> {});
            }

            AND_WHEN("All edges are enumerated")
            {
                auto edges = mrf.getEdges();

                THEN("The two expected edges are returned")
                {
                    REQUIRE(edges.size() == 2);

                    auto idxLess = [] (const auto& l, const auto& r) { return l.index < r.index; };
                    REQUIRE(std::is_sorted(begin(edges), end(edges), idxLess));

                    REQUIRE(edges.at(0).desc == EdgeDesc(pixel1, pixel2));
                    REQUIRE(edges.at(1).desc == EdgeDesc(pixel1, pixel3));
                }
            }

        }
    }
}
