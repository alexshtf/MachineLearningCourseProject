#ifndef GRIDMRF_H
#define GRIDMRF_H

#include "PixelsLabelsArray.h"
#include "hashtableview_fwd.h"

#define BOOST_HASH_NO_EXTENSIONS
#include <boost/functional/hash/hash.hpp>
#include <tuple>
#include <vector>

class Pixel
{
public:
    Pixel() = default;
    Pixel(size_t row, size_t col) : _row(row), _col(col) {}

    size_t row() const { return _row; }
    size_t col() const { return _col; }

    bool operator==(const Pixel& other) const { return tie() == other.tie(); }
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
    typedef decltype(tie) TieType;

    size_t _row;
    size_t _col;
};

class GridMRF
{
public:
    GridMRF(size_t rows, size_t cols, size_t labels, size_t neighborsCapacity);

    // potential setters
    void setUnary(Pixel pixel, size_t label, double value);
    void setPairwise(Pixel pixel1, Pixel pixel2, double value);

    // potential queries
    double getUnary(Pixel pixel, size_t label) const;
    double getPairwise(Pixel pixel1, Pixel pixel2, size_t label1, size_t label2) const;

    // dimensions
    size_t cols() const { return _cols; }
    size_t rows() const { return _rows; }
    size_t labels() const { return _labels; }
    size_t neighborsCapacity() const { return _neighborsCapacity; }
private:
    struct EdgeCell
    {
        bool empty;
        Pixel neighbor;
        double potential;

        EdgeCell() : empty(true) {}
    };
    struct CellTraits;
    typedef HashTableView<EdgeCell, CellTraits> CellsHashTable;
    typedef HashTableView<const EdgeCell, CellTraits> ConstCellsHashTable;

    size_t edgesIndexOf(Pixel pixel) const { return pixel.col() * (_rows * _neighborsCapacity) + pixel.row() * _neighborsCapacity; }
    EdgeCell* edgesPtrOf(Pixel pixel) { return &_edges[edgesIndexOf(pixel)]; }
    const EdgeCell* edgesPtrOf(Pixel pixel) const { return &_edges[edgesIndexOf(pixel)]; }

    size_t _rows;
    size_t _cols;
    size_t _labels;
    size_t _neighborsCapacity;

    Common::PixelsLabelsArray _unary;
    std::vector<EdgeCell> _edges;
};

#endif // GRIDMRF_H
