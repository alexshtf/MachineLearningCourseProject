#ifndef MRFMAP_H
#define MRFMAP_H

#include "gridmrf.h"
#include <boost/multi_array.hpp>

class MRFMap
{
public:
    MRFMap(const GridMRF& mrf);

private:
    boost::multi_array<double, 4> dualVariables; //
};

#endif // MRFMAP_H
