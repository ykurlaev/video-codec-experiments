#ifndef CODEC_INTERPOLATOR_H
#define CODEC_INTERPOLATOR_H

#include <vector>
#include <iterator>
#include "frame.h"
#include "util.h"

namespace Codec
{

class Interpolator
{
    private:
        typedef Frame<>::coord_t coord_t;
        typedef MakeSigned<coord_t>::t scoord_t;
        typedef Frame<>::data_t data_t;
        class Iterator;
    public:
        Interpolator(coord_t width, coord_t height, int scale = 2 /*###*/);
        template <typename Iter>
        void fillFrom(Iter begin, Iter end);
        Iterator regionBegin(coord_t x, coord_t y,
                             scoord_t xOffset, scoord_t yOffset,
                             coord_t width, coord_t height);
        Iterator regionEnd();
    private:
        coord_t m_width;
        coord_t m_height;
        int m_scale;
        std::vector<data_t> m_source;
        std::vector<data_t> m_interpolated;
};

class Interpolator::Iterator
    : public std::iterator<std::forward_iterator_tag, data_t>
{
    public:
        Iterator(data_t *ptr = NULL, scoord_t x = 0, scoord_t y = 0, coord_t width = 0, coord_t height = 0,
                 scoord_t xstart = 0, scoord_t xend = 0, scoord_t yend = 0, coord_t skip = 0, int scale = 0);
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        data_t &operator*();
        Iterator &operator++();
        Iterator operator++(int);
    private:
        data_t *m_ptr;
        scoord_t m_x;
        scoord_t m_y;
        coord_t m_width;
        coord_t m_height;
        scoord_t m_xstart;
        scoord_t m_xend;
        scoord_t m_yend;
        coord_t m_skip;
        int m_scale;
};

}

#include "interpolator.inl"

#endif //CODEC_INTERPOLATOR_H