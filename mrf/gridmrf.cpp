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

void GridMRF::setUnary(Pixel pixel, std::initializer_list<double> values)
{
    size_t i = 0;
    for(auto& val : values)
        setUnary(pixel, i++, val);
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

std::vector<EdgeInfo> GridMRF::getEdges() const
{
    // get a vector of all edges
    std::vector<EdgeInfo> edges;
    for(size_t i = 0; i < _edges.num_elements(); ++i)
    {
        // skip empty edges
        if ((_edges.origin() + i)->empty)
            continue;

        edges.emplace_back(getEdgeKey(i), i);
    }

    // sort by pixels and then by index
    auto idxLess = [] (const auto& l, const auto& r) { return l.index < r.index; };
    auto descLess = [] (const auto& l, const auto& r) { return l.desc < r.desc; };
    std::sort(begin(edges), end(edges), idxLess);
    std::stable_sort(begin(edges), end(edges), descLess);

    // remove edges with duplicate keys - stable-sorting above ensures
    // that the lowest index for each edge is used
    auto descEq = [] (const auto& l, const auto& r) { return l.desc == r.desc; };
    auto uniqueEnd = std::unique(begin(edges), end(edges), descEq);
    edges.erase(uniqueEnd, end(edges));

    // sort edges by index and return them
    std::sort(begin(edges), end(edges), idxLess);
    return edges;
}

EdgeInfo GridMRF::getEdgeInfo(const Pixel &from, const Pixel &to) const
{
    EdgeDesc desc(from, to);
    auto edge = ConstCellsHashTable(edgesPtrOf(desc.first()), _neighborsCapacity).find(desc.second());
    auto idx = edge - _edges.origin();

    return EdgeInfo(desc, idx);
}

EdgeDesc GridMRF::getEdgeKey(size_t edgeIndex) const
{
    size_t rowStride = _cols * _neighborsCapacity;
    size_t colStride = _neighborsCapacity;
    Pixel from(
         edgeIndex / rowStride,
         (edgeIndex % rowStride) / colStride);
    auto& to = (_edges.origin() + edgeIndex)->neighbor;
    return EdgeDesc(from, to);
}

boost::iterator_range<GridMRF::NeighborIterator> GridMRF::neighbors(const Pixel &pixel) const
{
    auto origin = _edges.origin();
    auto offset = edgesPtrOf(pixel) - origin;
    NeighborIterator begin(origin, _neighborsCapacity, offset);
    NeighborIterator end(origin, 0, offset + _neighborsCapacity);

    return boost::make_iterator_range(begin, end);
}
