#ifndef CODEC_REGION_H
#define CODEC_REGION_H

#include <istream>
#include <ostream>
#include <iterator>
#include "regionbuffer.h"
#include "../util.h"

namespace Codec
{

class Region
{
    public:
        typedef RegionBuffer::coord_t coord_t;
        typedef RegionBuffer::scoord_t scoord_t;
        typedef RegionBuffer::data_t data_t;
        template <typename U>
        class base_iterator;
        typedef base_iterator<const data_t> const_iterator;
        typedef base_iterator<data_t> iterator;
        Region(RegionBuffer *pRegionBuffer, coord_t xstart, coord_t ystart,
               coord_t width, coord_t height, coord_t tstart = 0);
	    Region(const Region &other);
        Region& operator=(const Region &other);
	    coord_t getWidth() const;
	    coord_t getHeight() const;
        const data_t &operator()(coord_t x, coord_t y) const;
	    data_t &operator()(coord_t x, coord_t y);
        const_iterator begin() const;
        iterator begin();
        const_iterator end() const;
    private:
	    coord_t m_width;
	    coord_t m_height;
        std::vector<data_t *> m_rows;
        template <typename T>
        friend class base_iterator;
        friend void swap(Region &first, Region &second);
};

void swap(Region &first, Region &second);

template <typename T>
class Region::base_iterator : public std::iterator<std::forward_iterator_tag, T>
{
    public:
        base_iterator();
        base_iterator(const base_iterator &other);
        template <typename U>
        base_iterator(const base_iterator<U> &other);
        base_iterator &operator=(const base_iterator &other);
        template <typename U>
        base_iterator &operator=(const base_iterator<U> &other);
        template <typename U>
        bool operator==(const base_iterator<U>& other) const;
        template <typename U>
        bool operator!=(const base_iterator<U>& other) const;
        typename base_iterator<T>::reference operator*();
        base_iterator &operator++();
        base_iterator operator++(int);
    private:
        base_iterator(typename CopyConst<T, Region>::t &region);
	    coord_t m_width;
	    coord_t m_height;
        typename CopyConst<T, Region::data_t>::t * const *m_rows;
        typename CopyConst<T, Region::data_t>::t *m_ptr;
        Region::coord_t m_x;
        Region::coord_t m_y;
        friend class Region;
        template <typename U>
        friend class base_iterator;
        template <typename T>
        friend void swap(base_iterator<T> &first, base_iterator<T> &second);
};

template <typename T>
void swap(Region::base_iterator<T> &first, Region::base_iterator<T> &second);

}

#include "region.inl"

#endif //CODEC_REGION_H