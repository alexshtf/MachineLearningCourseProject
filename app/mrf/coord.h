#ifndef COORD_H
#define COORD_H

#include <boost/functional/hash/hash.hpp>
#include <tuple>
#include <algorithm>
#include <iosfwd>

// Coordinates of a pixel. Contains all facilities for it to be used in an STL container,
// including equality, inequality, hash and less-than operators.
class Pixel
{
public:
    Pixel() = default;
    Pixel(size_t row, size_t col) : _row(row), _col(col) {}

    size_t row() const { return _row; }
    size_t col() const { return _col; }

    bool operator==(const Pixel& other) const { return tie() == other.tie(); }
    bool operator!=(const Pixel& other) const { return tie() != other.tie(); }
    bool operator<(const Pixel& other) const { return tie() < other.tie(); }

    size_t hash() const
    {
        size_t seed = 0;
        boost::hash_combine(seed, _row);
        boost::hash_combine(seed, _col);
        return seed;
    }

private:
    std::tuple<const size_t&, const size_t&> tie() const { return std::tie(_row, _col); }

    size_t _row;
    size_t _col;
};


// A canonical representation of an undirected edge endpoints in an MRF whose vertices are pixels.
//      EdgeEndpoints(p1, p2) and EdgeEndPoints(p2, p1) are equal
// Inludes all facilities required to be useful inside STL containers: equality, less-than, hash
class EdgeEndpoints
{
public:
    EdgeEndpoints(Pixel left, Pixel right)
    {
        std::tie(_first, _second) = std::minmax(left, right);
    }

    const Pixel& first() const { return _first; }
    const Pixel& second() const { return _second; }

    bool operator==(const EdgeEndpoints& other) const { return tie() == other.tie(); }
    bool operator!=(const EdgeEndpoints& other) const { return tie() != other.tie(); }
    bool operator<(const EdgeEndpoints& other) const { return tie() < other.tie(); }

    size_t hash() const
    {
        size_t seed = 0;
        boost::hash_combine(seed, _first.hash());
        boost::hash_combine(seed, _second.hash());
        return seed;
    }
private:
    std::tuple<const Pixel&, const Pixel&> tie() const { return std::tie(_first, _second); }

    Pixel _first;
    Pixel _second;
};

inline std::ostream& operator<<(std::ostream& os, const Pixel& pixel)
{
    os << "Pixel"
          " { row = " << pixel.row() <<
          " , col = " << pixel.col() <<
          " }";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const EdgeEndpoints& edgeKey)
{
    os << "EdgeKey"
          " { first = " << edgeKey.first() <<
          " , second = " << edgeKey.second() <<
          " }";
    return os;
}

namespace std
{
    template<> struct hash<Pixel>
    {
        size_t operator()(const Pixel& pixel) { return pixel.hash(); }
    };

    template<> struct hash<EdgeEndpoints>
    {
        size_t operator()(const EdgeEndpoints& edgeKey) { return edgeKey.hash(); }
    };
}


#endif // COORD_H

