#include "mrfmap.h"
#include "gridmrf.h"
#include <catch.hpp>

struct MRFMapImpl : public MRFMap
{
    MRFMapImpl(const GridMRF& mrf)
        : MRFMap(mrf)
    {}

    void init() override
    {
        MRFMap::init();
    }

    void nextIteration()
    {
        // do nothing
    }
};

SCENARIO("MRF-MAP base class provides common services")
{
    GIVEN("A Grid-MRF")
    {
        size_t rows = 2;
        size_t cols = 2;
        size_t labels = 4;
        size_t neighborsCapacity = 2;
        GridMRF mrf(rows, cols, labels, neighborsCapacity);

        WHEN("Unary and pairwise potentials are set")
        {
            mrf.setUnary(Pixel(0,0), {2.0, 3.0, 1.0, 6.0}); // min is label 2
            mrf.setUnary(Pixel(1,0), {2.0, 3.0, 4.0, 6.0}); // min is label 0
            mrf.setUnary(Pixel(0,1), {3.0, 2.0, 4.0, 6.0}); // min is label 1
            mrf.setUnary(Pixel(1,1), {3.0, 2.0, 4.0, 1.0}); // min is label 3

            mrf.setPairwise(Pixel(0,0), Pixel(1,0), 13.0);
            mrf.setPairwise(Pixel(0,0), Pixel(0,1), 17.0);

            AND_WHEN("MRF-MAP Initialization is performed")
            {
                MRFMapImpl mrfMap(mrf);
                mrfMap.init();

                THEN("Primal solution is the local minimizer of each unary potential")
                {
                    auto primal = mrfMap.primal();
                    CHECK(primal[0][0] == 2);
                    CHECK(primal[1][0] == 0);
                    CHECK(primal[0][1] == 1);
                    CHECK(primal[1][1] == 3);

                    AND_THEN("Primal energy is computed accordingly")
                    {
                        auto expectedEnergy
                                = mrf.getUnary(Pixel(0,0), primal[0][0])
                                + mrf.getUnary(Pixel(0,1), primal[0][1])
                                + mrf.getUnary(Pixel(1,0), primal[1][0])
                                + mrf.getUnary(Pixel(1,1), primal[1][1])
                                + mrf.getPairwise(Pixel(0,0), Pixel(1,0), primal[0][0], primal[1][0])
                                + mrf.getPairwise(Pixel(0,0), Pixel(0,1), primal[0][0], primal[0][1]);

                        REQUIRE(mrfMap.computePrimalEnergy() == expectedEnergy);
                    }
                }

                THEN("Dual energy is below primal energy")
                {
                    REQUIRE(mrfMap.computePrimalEnergy() > mrfMap.computeDualEnergy());
                }
            }
        }


    }
}
