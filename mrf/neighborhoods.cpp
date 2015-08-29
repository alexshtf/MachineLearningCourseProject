#include "neighborhoods.h"

namespace {
    size_t FIRST_BIT = 1;
    size_t SECOND_BIT = 2;
}


FourNeighbors::FourNeighbors(size_t rows, size_t cols, const Pixel &pixel)
    : _rows(rows)
    , _cols(cols)
    , _center(pixel)
{}


FourNeighbors::iterator FourNeighbors::begin() const
{
    return iterator(_rows, _cols, _center);
}

FourNeighbors::iterator FourNeighbors::end() const
{
    return iterator();
}


Pixel FourNeighbors::iterator::operator*() const
{
    return Pixel(_center.row() + dRow(), _center.col() + dCol());
}

bool FourNeighbors::iterator::operator==(const FourNeighbors::iterator &other) const
{
    return _curr == other._curr;
}

bool FourNeighbors::iterator::operator!=(const FourNeighbors::iterator &other) const
{
    return _curr != other._curr;
}

FourNeighbors::iterator &FourNeighbors::iterator::operator++()
{
    advanceOnce();
    advanceToValid();
    return *this;
}

FourNeighbors::iterator::iterator(size_t rows, size_t cols, const Pixel &center)
    : _rows(rows)
    , _cols(cols)
    , _center(center)
    , _curr(0)
{
    advanceToValid();
}

FourNeighbors::iterator::iterator()
    : _curr(MAX_NEIGHBORS)
{
}

void FourNeighbors::iterator::advanceToValid()
{
    while (canAdvance() && isCurrInvalid())
        advanceOnce();
}

bool FourNeighbors::iterator::canAdvance() const
{
    return _curr < MAX_NEIGHBORS;
}

bool FourNeighbors::iterator::isCurrInvalid() const
{
    auto invalid = [] (int diff, size_t center, size_t max) {
        return
            (diff == -1 && center == 0) ||    // low boundary
            (diff == 1 && center == max - 1); // high boundary
    };

    if (invalid(dRow(), _center.row(), _rows))
        return true;
    if (invalid(dCol(), _center.col(), _cols))
        return true;

    return false;
}

void FourNeighbors::iterator::advanceOnce()
{
    ++_curr;
}

int FourNeighbors::iterator::dRow() const
{
    if (_curr == 0)
        return -1;
    if (_curr == 2)
        return 1;
    return 0;
}

int FourNeighbors::iterator::dCol() const
{
    if (_curr == 1)
        return -1;
    if (_curr == 3)
        return 1;
    return 0;
}
