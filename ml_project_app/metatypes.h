#ifndef METATYPES_H
#define METATYPES_H

#include <boost/multi_array.hpp>
#include <boost/utility/identity_type.hpp>
#include <QMetaType>

Q_DECLARE_METATYPE(BOOST_IDENTITY_TYPE( (boost::multi_array<size_t, 2>) ))

void registerMetatypes();

#endif // METATYPES_H

