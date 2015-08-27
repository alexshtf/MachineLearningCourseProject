#include "mrfmap.h"
#include <algorithm>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm/min_element.hpp>

namespace {

size_t maxNumOfEdges(const GridMRF& mrf)
{
    return mrf.rows() * mrf.cols() * mrf.neighborsCapacity();
}

}

MRFMap::MRFMap(const GridMRF &mrf)
    : _mrf(mrf)
    , _dualVariables(boost::extents[maxNumOfEdges(mrf)][2][mrf.labels()])
    , _primalVariables(boost::extents[mrf.rows()][mrf.cols()])
{
}

double MRFMap::computePrimalEnergy() const
{
    auto sum = 0.0;

    // accumulate unary potentials
    for(size_t r = 0; r < _mrf.rows(); ++r)
        for(size_t c = 0; c < _mrf.cols(); ++c)
            sum += _mrf.getUnary(Pixel(r, c), _primalVariables[r][c]);

    // accumulate pairwise potentials
    for(const EdgeInfo& edge : _mrf.getEdges())
    {
        auto& pix1 = edge.desc.first();
        auto& pix2 = edge.desc.second();
        sum += _mrf.getPairwise(edge.index, primalAt(pix1), primalAt(pix2));
    }

    return sum;
}

double MRFMap::computeDualEnergy() const
{
    return 0;
}

void MRFMap::init()
{
    std::fill_n(_dualVariables.origin(), _dualVariables.num_elements(), 0.0);
    setPrimalVariablesToUnaryMinimizers();
}

void MRFMap::setPrimalVariablesToUnaryMinimizers()
{
    auto labels = boost::irange((size_t) 0, _mrf.labels());
    for(size_t r = 0; r < _mrf.rows(); ++r)
    {
        for(size_t c = 0; c < _mrf.cols(); ++c)
        {
            Pixel pixel(r, c);
            _primalVariables[r][c] = *boost::min_element(labels, [&] (auto l1, auto l2) {
                return _mrf.getUnary(pixel, l1) < _mrf.getUnary(pixel, l2);
            });
        }
    }
}

size_t MRFMap::primalAt(const Pixel &pixel) const
{
    return _primalVariables[pixel.row()][pixel.col()];
}
