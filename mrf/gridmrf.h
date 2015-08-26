#ifndef GRIDMRF_H
#define GRIDMRF_H

#include "coord.h"
#include "PixelsLabelsArray.h"
#include "hashtableview_fwd.h"

#include <boost/range/iterator_range.hpp>
#define BOOST_HASH_NO_EXTENSIONS
#include <boost/multi_array.hpp>

#include <iterator>

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
    double getPairwise(size_t edgeIndex, size_t label1, size_t label2) const;

    // edge queries
    std::vector<size_t> getEdgeIndices() const;
    EdgePixels getEdgeKey(size_t edgeIndex) const;

    // dimensions
    size_t cols() const { return _cols; }
    size_t rows() const { return _rows; }
    size_t labels() const { return _labels; }
    size_t neighborsCapacity() const { return _neighborsCapacity; }\

private:
    struct EdgeCell
    {
        bool empty;
        Pixel neighbor;
        double potential;

        EdgeCell() : empty(true) {}
    };

public:

    struct NeighborIterator : public std::iterator<std::forward_iterator_tag, Pixel>
    {
        friend class GridMRF;
        NeighborIterator() = default;

        size_t edgeIndex() const { return _offset; }
        const Pixel& operator*() const { return _cells[_offset].neighbor; }

        NeighborIterator& operator++()
        {
            advanceOnce();
            advanceToNonEmpty();
            return *this;
        }

        bool operator==(const NeighborIterator& other) { return tie() == other.tie(); }
        bool operator!=(const NeighborIterator& other) { return tie() != other.tie(); }

    private:
        NeighborIterator(EdgeCell* cells, size_t capacity, size_t offset)
            : _cells(cells)
            , _capacity(capacity)
            , _offset(offset)
        {
            advanceToNonEmpty();
        }

        void advanceToNonEmpty()
        {
            while (_capacity > 0 && _cells[_offset].empty)
            {
                ++_offset;
                --_capacity;
            }
        }

        void advanceOnce()
        {
            ++_offset;
            --_capacity;
        }

        std::tuple<EdgeCell* const &, const size_t&, const size_t&> tie() const
        {
            return std::tie(_cells, _capacity, _offset);
        }


        EdgeCell* _cells;
        size_t _capacity;
        size_t _offset;
    };

    // iteration
    boost::iterator_range<NeighborIterator> neighbors(const Pixel& pixel);

private:

    struct CellTraits;
    typedef HashTableView<EdgeCell, CellTraits> CellsHashTable;
    typedef HashTableView<const EdgeCell, CellTraits> ConstCellsHashTable;

    EdgeCell* edgesPtrOf(Pixel pixel) { return &_edges[pixel.row()][pixel.col()][0]; }
    const EdgeCell* edgesPtrOf(Pixel pixel) const { return &_edges[pixel.row()][pixel.col()][0]; }

    size_t _rows;
    size_t _cols;
    size_t _labels;
    size_t _neighborsCapacity;

    Common::PixelsLabelsArray _unary;
    boost::multi_array<EdgeCell, 3> _edges; // row x col x neighbors_capacity
};

#endif // GRIDMRF_H
