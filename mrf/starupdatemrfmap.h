#ifndef STARUPDATEMRFMAP_H
#define STARUPDATEMRFMAP_H

#include "mrfmap.h"
#include <boost/multi_array.hpp>

class StarUpdateMRFMap : MRFMap
{
public:
    StarUpdateMRFMap(const GridMRF& mrf);

    void init() override;
    void nextIteration() override;

private:
    typedef boost::multi_array<double, 2> work2d;
    typedef std::vector<double> work1d;

    void starUpdate(const Pixel& pixel, work2d& deltaMinus, work1d& omega, work2d& omegaJ);
    size_t ls() const { return _mrf.labels(); }
    size_t rs() const { return _mrf.rows(); }
    size_t cs() const { return _mrf.cols(); }
    size_t ns() const { return _mrf.neighborsCapacity(); }
};

#endif // STARUPDATEMRFMAP_H
