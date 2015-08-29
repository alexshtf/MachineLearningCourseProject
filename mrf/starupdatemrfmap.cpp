#include "starupdatemrfmap.h"
#include "neighborhoods.h"
#include <boost/multi_array.hpp>

StarUpdateMRFMap::StarUpdateMRFMap(const GridMRF &mrf)
    : MRFMap(mrf)
{
}


void StarUpdateMRFMap::init()
{
    MRFMap::init();
}

void StarUpdateMRFMap::nextIteration()
{
    auto ls = _mrf.labels();

    work2d deltaMinus(boost::extents[ns][ls]);
    work1d omega(ls);
    work2d omegaJ(boost::extents[ns][ls]);

    for(auto r = 0; r < _rows; ++r)
        for(auto c = 0; c < _cols; ++c)
            starUpdate(Pixel(r, c), deltaMinus, omega, omegaJ);
}

void StarUpdateMRFMap::starUpdate(const Pixel &pixel, work2d& deltaMinus, work1d& omega, work2d& omegaJ)
{
    // global params
    auto rs = _mrf.rows();
    auto cs = _mrf.cols();
    auto ls = _mrf.labels();

    // get neighbors
    Pixel neighbors[4];
    size_t n = 0;
    for(const auto& neighbor : FourNeighbors(rs, cs, pixel))
        neighbors[n++] = neighbor;

    // compute delta_minus
    for(size_t j = 0; j < n; ++j)
    {
        for(size_t x = 0; x < ls; ++)
        {
            deltaMinus[j][x] = _mrf.getUnary(neighbors[j], x);
            for()
        }
    }
}
