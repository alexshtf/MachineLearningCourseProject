#include "starupdatemrfmap.h"
#include "neighborhoods.h"
#include "Util.h"
#include <boost/multi_array.hpp>
#include <boost/range/irange.hpp>

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
    work2d deltaMinus(boost::extents[ns()][ls()]);
    work1d omega(ls());
    work2d omegaJ(boost::extents[ns()][ls()]);

    for(auto r = 0; r < rs(); ++r)
        for(auto c = 0; c < cs(); ++c)
            starUpdate(Pixel(r, c), deltaMinus, omega, omegaJ);
}

void StarUpdateMRFMap::starUpdate(const Pixel &pixel, work2d& deltaMinus, work1d& omega, work2d& omegaJ)
{
    // global params
    auto lsRng = boost::irange(static_cast<size_t>(0), ls());

    // get neighbors
    Pixel neighbors[4];
    size_t n = 0; // number of neighbors
    for(const auto& neighbor : FourNeighbors(rs(), cs(), pixel))
        neighbors[n++] = neighbor;

    // compute delta_minus
    for(size_t j = 0; j < n; ++j)
    {
        const auto& nj = neighbors[j];
        for(auto x : lsRng)
        {
            deltaMinus[j][x] = _mrf.getUnary(nj, x);
            for(const auto& nk : _mrf.neighbors(neighbors[j]))
                if (nk != pixel)
                    deltaMinus[j][x] += dualAt(EdgeDesc(nk, nj), nj, x);
        }
    }

    // compute omegaJ
    for (size_t j = 0; j < n; ++j)
    {
        const auto& nj = neighbors[j];
        for(auto xi : lsRng)
        {
            omegaJ[j][xi] = max_value(lsRng, [&] (size_t xj) {
                return _mrf.getPairwise(pixel, nj, xi, xj) + deltaMinus[j][xj];
            });
        }
    }

    // compute omega
    for(auto xi : lsRng)
    {
        omega[xi] = _mrf.getUnary(pixel, xi);
        for(size_t j = 0; j < n; ++j)
            omega[xi] += omegaJ[j][xi];
    }

    // perform the star update
    double coeff = 1.0 / (1 + n);
    for(size_t j = 0; j < n; ++j)
    {
        const auto& nj = neighbors[j];
        for(auto xi : lsRng)
            dualAt(EdgeDesc(pixel, nj), pixel, xi) = -coeff * omega[xi]  + omegaJ[j][xi];

        for(auto xj : lsRng)
        {
            auto m = max_value(lsRng, [&] (size_t xi) {
                return _mrf.getPairwise(pixel, nj, xi, xj) + 2 * coeff * omega[xi] - omegaJ[j][xi];
            });
            dualAt(EdgeDesc(pixel, nj), nj, xj) = -0.5 * deltaMinus[j][xj] + 0.5 * m;
        }

    }
}
