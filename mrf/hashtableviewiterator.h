#ifndef HASHTABLEVIEWITERATOR
#define HASHTABLEVIEWITERATOR

template<typename Cell, typename CellTraits>
class HashTableViewIterator
{
private:
    Cell* _cells;
    size_t _offset;
    size_t _capacity;

public:
    HashTableViewIterator() = default;
    HashTableViewIterator(Cell* cells, size_t capacity)
        : _cells(cells)
        , _offset(0)
        , _capacity(capacity)
    {
        advanceToNonempty();
    }

    HashTableViewIterator& operator++()
    {
        ++_offset;
        advanceToNonempty();
    }

    Cell& operator*() const { return _cells[_offset]; }
    Cell* operator->() const { return _cells + _offset; }

private:
    void advanceToNonempty()
    {
        while (empty(_cells(_offset)) && _offset < _capacity)
            ++_offset;
    }

    static bool empty(const Cell& cell) { return CellTraits::empty(cell); }
};

#endif // HASHTABLEVIEWITERATOR

