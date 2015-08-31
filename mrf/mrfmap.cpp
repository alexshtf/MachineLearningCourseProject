#include "mrfmap.h"
#include <algorithm>
#include "Util.h"
#include <boost/range/irange.hpp>
#include <boost/range/algorithm/max_element.hpp>

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
    for(size_t r = 0; r < rs(); ++r)
        for(size_t c = 0; c < cs(); ++c)
            sum += _mrf.getUnary(Pixel(r, c), _primalVariables[r][c]);

    // accumulate pairwise potentials
    for(const auto& edge : _mrf.getEdges())
    {
        auto& pix1 = edge.desc.first();
        auto& pix2 = edge.desc.second();
        sum += _mrf.getPairwise(edge.index, primalAt(pix1), primalAt(pix2));
    }

    return sum;
}

double MRFMap::computeDualEnergy() const
{
    auto sum = 0.0;

    // accumulate unary maxima
    size_t dummyMaxLabel;
    for(size_t r = 0; r < rs(); ++r)
        for(size_t c = 0; c < cs(); ++c)
            sum += unaryMax(Pixel(r, c), dummyMaxLabel);

    // accumulaty pairwise maxima
    for(const auto& edge : _mrf.getEdges())
        sum += pairwiseMax(edge);

    return sum;
}

void MRFMap::init()
{
    std::fill_n(_dualVariables.origin(), _dualVariables.num_elements(), 0.0);
    setPrimalVariablesToUnaryMaximizers();
}

void MRFMap::setPrimalVariablesToUnaryMaximizers()
{
    for(size_t r = 0; r < rs(); ++r)
        for(size_t c = 0; c < cs(); ++c)
            unaryMax(Pixel(r, c), _primalVariables[r][c]);
}

size_t MRFMap::primalAt(const Pixel &pixel) const
{
    return _primalVariables[pixel.row()][pixel.col()];
}

double& MRFMap::dualAt(const EdgeDesc &edge, const Pixel &at, size_t label)
{
    auto edgeInfo = _mrf.getEdgeInfo(edge.first(), edge.second());
    auto pixelIdx = edgeInfo.desc.first() == at ? 0 : 1;
    return _dualVariables[edgeInfo.index][pixelIdx][label];
}

double MRFMap::unaryMax(const Pixel &pixel, size_t& maxLabel) const
{
    double maxEnergy;

    std::tie(maxEnergy, maxLabel) = max_and_argmax(labelRange(), [&] (size_t l) {
        double energy = _mrf.getUnary(pixel, l);
        for(const auto& neighbor : _mrf.neighbors(pixel))
        {
            auto edgeInfo = _mrf.getEdgeInfo(pixel, neighbor);
            auto pixelIndex = pixel == edgeInfo.desc.first() ? 0 : 1;
            energy += _dualVariables[edgeInfo.index][pixelIndex][l];
        }
        return energy;
    });

    return maxEnergy;
}

double MRFMap::pairwiseMax(const EdgeInfo &edge) const
{
    return max_value(labelRange(), [&] (size_t l1) {
        return max_value(labelRange(), [&] (size_t l2) {
            return _mrf.getPairwise(edge.index, l1, l2)
                + _dualVariables[edge.index][0][l1]
                + _dualVariables[edge.index][1][l2];
        });
    });
}

