#include <exception>
#include <algorithm>
#include <cstring>

namespace Codec
{

inline Region::Region(RegionBuffer *pRegionBuffer, coord_t xstart, coord_t ystart,
                      coord_t width, coord_t height, coord_t tstart)
    : m_width(width), m_height(height), m_rows(height)
{
    for(size_t i = 0; i < height; i++)
    {
        m_rows[i] = &(*pRegionBuffer)(xstart, ystart + i, tstart);
    }
}

inline Region::Region(const Region &other)
    : m_width(other.m_width), m_height(other.m_height), m_rows(other.m_rows)
{}

inline Region &Region::operator=(const Region &other)
{
    Region tmp(other);
    swap(*this, tmp);
    return *this;
}

inline Region::coord_t Region::getWidth() const
{
    return m_width;
}

inline Region::coord_t Region::getHeight() const
{
    return m_height;
}

inline const Region::data_t &Region::operator()(coord_t x, coord_t y) const
{
    assert(x < m_width && y < m_height);
    return m_rows[y][x];
}

inline Region::data_t &Region::operator()(coord_t x, coord_t y)
{
    assert(x < m_width && y < m_height);
    return m_rows[y][x];
}

inline Region::const_iterator Region::begin() const
{
    return const_iterator(*this);
}

inline Region::iterator Region::begin()
{
    return iterator(*this);
}

inline Region::const_iterator Region::end() const
{
    return const_iterator();
}

inline void swap(Region &first, Region &second)
{
    using std::swap;
    swap(first.m_width, second.m_width);
    swap(first.m_height, second.m_height);
    swap(first.m_rows, second.m_rows);
}

template <typename T>
inline Region::base_iterator<T>::base_iterator()
    : m_width(0), m_height(0), m_rows(NULL), m_ptr(NULL), m_x(0), m_y(0)
{}

template <typename T>
inline Region::base_iterator<T>::base_iterator(const base_iterator &other)
    : m_width(other.m_width), m_height(other.m_height), m_rows(other.m_rows),
      m_ptr(other.m_ptr), m_x(0), m_y(0)
{}

template <typename T>
template <typename U>
inline Region::base_iterator<T>::base_iterator(const base_iterator<U> &other)
    : m_width(other.m_width), m_height(other.m_height), m_rows(other.m_rows),
      m_ptr(other.m_ptr), m_x(0), m_y(0)
{}

template <typename T>
inline Region::base_iterator<T> &Region::base_iterator<T>::operator=(const base_iterator &other)
{
    return this->operator=<>(other);
}

template <typename T>
template <typename U>
inline Region::base_iterator<T> &Region::base_iterator<T>::operator=(const base_iterator<U> &other)
{
    base_iterator tmp(other);
    swap(*this, tmp);
    return *this;
}

template <typename T>
template <typename U>
inline bool Region::base_iterator<T>::operator==(const base_iterator<U>& other) const
{
    return m_ptr == other.m_ptr;
}

template <typename T>
template <typename U>
inline bool Region::base_iterator<T>::operator!=(const base_iterator<U>& other) const
{
    return !(*this == other);
}

template <typename T>
inline typename Region::base_iterator<T>::reference Region::base_iterator<T>::operator*()
{
    return *m_ptr;
}

template <typename T>
inline Region::base_iterator<T> &Region::base_iterator<T>::operator++()
{
    if(m_ptr != NULL)
    {
        m_x++;
        if(m_x >= m_width)
        {
            m_x = 0;
            m_y++;
            if(m_y >= m_height)
            {
                m_y = 0;
                m_ptr = NULL;
            }
            else
            {
                m_ptr = m_rows[m_y];
            }
        }
        else
        {
            m_ptr++;
        }
    }
    return *this;
}

template <typename T>
inline typename Region::base_iterator<T> Region::base_iterator<T>::operator++(int)
{
    base_iterator result(*this);
    ++(*this);
    return result;
}

template <typename T>
inline Region::base_iterator<T>::base_iterator(typename CopyConst<T, Region>::t &region)
    : m_width(region.m_width), m_height(region.m_height), m_rows(&region.m_rows[0]),
      m_ptr(region.m_rows[0]), m_x(0), m_y(0)
{}

template <typename T>
inline void swap(Region::base_iterator<T> &first, Region::base_iterator<T> &second)
{
    using std::swap;
    swap(first.m_width, second.m_width);
    swap(first.m_height, second.m_height);
    swap(first.m_rows, second.m_rows);
    swap(first.m_ptr, second.m_ptr);
    swap(first.m_x, second.m_x);
    swap(first.m_y, second.m_y);
}

}