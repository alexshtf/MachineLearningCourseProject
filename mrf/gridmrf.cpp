#include "gridmrf.h"
#include "hashtableview.h"

struct GridMRF::CellTraits
{
    typedef Pixel Key;
    typedef EdgeCell Cell;

    static size_t hash(const Pixel& pixel) { return pixel.hash(); }
    static bool empty(const Cell& cell) { return cell.empty; }
    static Pixel key(const Cell& cell) { return cell.neighbor; }
    static void occupy(Cell& cell, const Key& key)
    {
        if (cell.empty)
        {
            cell.empty = false;
            cell.neighbor = key;
        }
    }
};

GridMRF::GridMRF(size_t rows, size_t cols, size_t labels, size_t neighborsCapacity)
    : _rows(rows)
    , _cols(cols)
    , _labels(labels)
    , _neighborsCapacity(neighborsCapacity)
    , _unary(rows, cols, labels)
    , _edges(boost::extents[rows][cols][neighborsCapacity])
{
}

void GridMRF::setUnary(Pixel pixel, size_t label, double value)
{
    _unary.At(pixel.row(), pixel.col(), label) = value;
}

void GridMRF::setPairwise(Pixel pixel1, Pixel pixel2, double value)
{
    CellsHashTable neighbors1(edgesPtrOf(pixel1), _neighborsCapacity);
    CellsHashTable neighbors2(edgesPtrOf(pixel2), _neighborsCapacity);

    auto edge12 = neighbors1.occupy(pixel2);
    auto edge21 = neighbors2.occupy(pixel1);

    edge12->potential = value;
    edge21->potential = value;
}

double GridMRF::getUnary(Pixel pixel, size_t label) const
{
    return _unary.At(pixel.row(), pixel.col(), label);
}

double GridMRF::getPairwise(Pixel pixel1, Pixel pixel2, size_t label1, size_t label2) const
{
    if (label1 == label2)
        return 0;

    ConstCellsHashTable neighbors(edgesPtrOf(pixel1), _neighborsCapacity);
    auto edge = neighbors.find(pixel2);
    return edge->potential;
}

double GridMRF::getPairwise(size_t edgeIndex, size_t label1, size_t label2) const
{
    if (label1 == label2)
        return 0;

    auto origin = _edges.origin();
    return origin[edgeIndex].potential;
}

std::vector<size_t> GridMRF::getEdgeIndices() const
{
    // get a vector of all edges
    std::vector<std::pair<EdgePixels, size_t>> edges;
    for(size_t i = 0; i < _edges.num_elements(); ++i)
    {
        // skip empty edges
        if ((_edges.origin() + i)->empty)
            continue;

        edges.emplace_back(getEdgeKey(i), i);
    }

    // remove duplicate edges (each edge appears twice)
    // and ensure that the lower index is used
    auto idxLess = [] (const auto& l, const auto& r) { return l.second < r.second; };
    auto keyLess = [] (const auto& l, const auto& r) { return l.first < r.first; };
    auto keyEq = [] (const auto& l, const auto& r) { return l.first == r.first; };
    std::sort(begin(edges), end(edges), idxLess);
    std::stable_sort(begin(edges), end(edges), keyLess);
    auto uniqueEnd = std::unique(begin(edges), end(edges), keyEq);

    // extract edge indices
    std::vector<size_t> indices(uniqueEnd - begin(edges));
    std::transform(begin(edges), uniqueEnd, begin(indices),[] (const auto& e) { return e.second; });
    std::sort(begin(indices), end(indices));
    return indices;
}

EdgePixels GridMRF::getEdgeKey(size_t edgeIndex) const
{
    size_t rowStride = _cols * _neighborsCapacity;
    size_t colStride = _neighborsCapacity;
    Pixel from(
         edgeIndex / rowStride,
         (edgeIndex % rowStride) / colStride);
    auto& to = (_edges.origin() + edgeIndex)->neighbor;
    return EdgePixels(from, to);
}

boost::iterator_range<GridMRF::NeighborIterator> GridMRF::neighbors(const Pixel &pixel)
{
    auto origin = _edges.origin();
    auto offset = edgesPtrOf(pixel) - origin;
    NeighborIterator begin(origin, _neighborsCapacity, offset);
    NeighborIterator end(origin, 0, offset + _neighborsCapacity);

    return boost::make_iterator_range(begin, end);
}
