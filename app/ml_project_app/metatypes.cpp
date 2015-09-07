#include "metatypes.h"

void registerMetatypes()
{
    qRegisterMetaType<boost::multi_array<size_t, 2>>();
}
