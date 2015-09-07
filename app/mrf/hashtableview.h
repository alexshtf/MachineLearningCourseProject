#ifndef ARRAYHASHTABLE
#define ARRAYHASHTABLE

template<typename Cell, typename CellTraits>
class HashTableView
{
public:
    typedef typename CellTraits::Key Key;

    HashTableView(Cell* cells, size_t capacity)
        : _cells(cells)
        , _capacity(capacity)
    {
    }

    Cell* find(const Key& key)
    {
        return linearSearch(index(key), [&] (const Cell& cell) {
            return occupied(cell) && key == keyOf(cell);
        });
    }

    Cell* occupy(const Key& key)
    {
        auto emptyCell = linearSearch(index(key), [&] (const Cell& cell) {
           return empty(cell) || keyOf(cell) == key;
        });
        occupy(*emptyCell, key);
        return emptyCell;
    }

private:
    static size_t hash(const Key& key) { return CellTraits::hash(key); }

    static Key keyOf(const Cell& cell) { return CellTraits::key(cell); }
    static bool empty(const Cell& cell) { return CellTraits::empty(cell); }
    static bool occupied(const Cell& cell) { return !empty(cell); }

    static void occupy(Cell& cell, const Key& key) { CellTraits::occupy(cell, key); }

    size_t index(const Key& key) { return hash(key) % _capacity; }

    template<typename Pred>
    Cell* linearSearch(size_t startAt, Pred pred)
    {
        for(size_t i = 0; i < _capacity; ++i)
        {
            auto idx = (i + startAt) % _capacity;
            if (pred(_cells[idx]))
                return _cells + idx;
        }
        return nullptr;
    }

    Cell* _cells;
    size_t _capacity;
};

#endif // ARRAYHASHTABLE

