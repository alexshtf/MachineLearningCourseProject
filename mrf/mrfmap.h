#ifndef MRFMAP_H
#define MRFMAP_H

#include "gridmrf.h"

class MRFMap
{
public:
    MRFMap(const GridMRF& mrf);

private:
    std::vector<double> _dualVariables;
};

#endif // MRFMAP_H
