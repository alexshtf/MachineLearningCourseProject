#ifndef MRFMAP_H
#define MRFMAP_H

#include "gridmrf.h"
#include <boost/multi_array.hpp>

class MRFMap
{
public:
    MRFMap(const GridMRF& mrf);

    // queries
    const boost::multi_array<size_t, 2> primal() const { return _primalVariables; }
    double computePrimalEnergy() const;
    double computeDualEnergy() const;

    // iteration
    virtual void init() = 0;
    virtual void nextIteration() = 0;
protected:
    void setPrimalVariablesToUnaryMinimizers();
    size_t primalAt(const Pixel& pixel) const;

    const GridMRF& _mrf;
    boost::multi_array<double, 3> _dualVariables; // Edges X 2 X Labels (since each edge is connected to two pixels)
    boost::multi_array<size_t, 2> _primalVariables; // label assignment for each pixel
};

#endif // MRFMAP_H
