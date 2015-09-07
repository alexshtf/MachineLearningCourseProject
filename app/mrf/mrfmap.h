#ifndef MRFMAP_H
#define MRFMAP_H

#include "gridmrf.h"
#include <boost/multi_array.hpp>
#include <boost/range/irange.hpp>

// A base class for a primal-dual MRF-MAP inference engine.
class MRFMap
{
public:
    MRFMap(const GridMRF& mrf);
    virtual ~MRFMap() = default;

    // queries
    const boost::multi_array<size_t, 2> primal() const { return _primalVariables; }
    double computePrimalEnergy() const;
    double computeDualEnergy() const;

    // iteration
    virtual void init() = 0;
    virtual void nextIteration() = 0;
    virtual void computePrimal();

protected:
    void computePrimalForCurrentDual();
    size_t primalAt(const Pixel& pixel) const;
    double &dualAt(const EdgeEndpoints& edge, const Pixel& at, size_t label);
    double unaryMax(const Pixel& pixel, size_t &maxLabel) const;
    double pairwiseMax(const EdgeInfo& edge) const;

    size_t rs() const { return _mrf.rows(); }
    size_t cs() const { return _mrf.cols(); }
    size_t ls() const { return _mrf.labels(); }
    size_t ns() const { return _mrf.neighborsCapacity(); }
    auto labelRange() const { return boost::irange((size_t)0, ls()); }

    const GridMRF& _mrf;
    boost::multi_array<double, 3> _dualVariables; // Edges X 2 X Labels (since each edge is connected to two pixels)
    boost::multi_array<size_t, 2> _primalVariables; // label assignment for each pixel
};

// an implementation of MRF-MAP where each iteration does nothing.
// useful for testing integration of MRF-MAP inside the application itself
class EmptyMRFMap : public MRFMap
{
public:
    EmptyMRFMap(const GridMRF& gridMrf)
        : MRFMap(gridMrf)
    {}

    void init() override  { MRFMap::init(); }
    void nextIteration() override { /* do nothing */ }
};

#endif // MRFMAP_H
