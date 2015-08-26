#include "mrfmap.h"
#include <algorithm>

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

}

double MRFMap::computeDualEnergy() const
{
    return 0;
}

void MRFMap::init()
{
    std::fill_n(_dualVariables.origin(), _dualVariables.num_elements(), 0.0);
    setPrimalVariablesToUnaryMaximizers();
}
