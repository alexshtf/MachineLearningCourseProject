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
};

#endif // STARUPDATEMRFMAP_H
