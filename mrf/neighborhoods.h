#ifndef NEIGHBORHOODS_H
#define NEIGHBORHOODS_H

#include "coord.h"
#include <iterator>

class FourNeighbors
{
public:
    FourNeighbors(size_t rows, size_t cols, const Pixel& pixel);

    struct iterator : public std::iterator<std::forward_iterator_tag, Pixel>
    {
        friend class FourNeighbors;
        Pixel operator*() const;
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
        iterator& operator++();
    private:
        iterator(size_t rows, size_t cols, const Pixel& center);
        iterator();

        void advanceToValid();
        bool canAdvance() const;
        bool isCurrInvalid() const;
        void advanceOnce();

        int dRow() const;
        int dCol() const;

        size_t _rows;
        size_t _cols;
        Pixel _center;
        size_t _curr;

        static const size_t MAX_NEIGHBORS = 4;
    };

    iterator begin() const;
    iterator end() const;
private:
    size_t _rows;
    size_t _cols;
    Pixel _center;
};

#endif // NEIGHBORHOODS_H
