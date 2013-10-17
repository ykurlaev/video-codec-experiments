#include <cassert>
#include <algorithm>
#include <cstring>

namespace Codec
{

template <uint32_t MIN_N, uint32_t MAX_N>
inline Frame<MIN_N, MAX_N>::Frame(coord_t width, coord_t height)
    : m_width(width), m_height(height),
      m_alignedWidth(((width + MAX_N - 1) / MAX_N) * MAX_N),
      m_alignedHeight(((height + MAX_N - 1) / MAX_N) * MAX_N),
      m_data(m_alignedWidth * m_alignedHeight), m_ptrs(m_alignedWidth * m_alignedHeight)
{
    assert(MIN_N != 0 && MAX_N >= MIN_N);
    coord_t blockX = 0, blockY = 0, i = 0;
    for(int *ptr = &m_data[0]; ptr != &m_data[0] + m_data.size(); ptr++)
    {
        coord_t x = i % MIN_N, y = (i / MIN_N) % MIN_N;
        m_ptrs[(blockY + y) * m_alignedWidth + blockX + x] = ptr;
        i++;
        if(i % (MIN_N * MIN_N) == 0)
        {
            coord_t block = i / (MAX_N * MAX_N);
            blockX = (block % (m_alignedWidth / MAX_N)) * MAX_N;
            blockY = (block / (m_alignedWidth / MAX_N)) * MAX_N;
            for(uint32_t size = MAX_N / 2; size >= MIN_N; size /= 2)
            {
                coord_t relBlock = (i % (size * size * 4)) / (size * size);
                blockX += (relBlock % 2) * size;
                blockY += (relBlock / 2) * size;
            }
        }
    }
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline coord_t Frame<MIN_N, MAX_N>::getWidth() const 
{
    return m_width;
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline coord_t Frame<MIN_N, MAX_N>::getHeight() const
{
    return m_height;
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline coord_t Frame<MIN_N, MAX_N>::getAlignedWidth() const
{
    return m_alignedWidth;
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline coord_t Frame<MIN_N, MAX_N>::getAlignedHeight() const
{
    return m_alignedHeight;
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline void Frame<MIN_N, MAX_N>::clear()
{
    std::memset(&m_data[0], 0, m_data.size() * sizeof(data_t));
}

template<uint32_t MIN_N, uint32_t MAX_N>
inline void Frame<MIN_N, MAX_N>::fromByteArray(const uint8_t *byteArray)
{
    data_t **pptr = &m_ptrs[0];
    for(size_t y = 0; y < m_height; y++)
    {
        for(size_t x = 0; x < m_width; x++)
        {
            *(*pptr++) = *byteArray++;
        }
        pptr += m_alignedWidth - m_width;
    }
}

template<uint32_t MIN_N, uint32_t MAX_N>
inline void Frame<MIN_N, MAX_N>::toByteArray(uint8_t *byteArray)
{
    data_t **pptr = &m_ptrs[0];
    for(size_t y = 0; y < m_height; y++)
    {
        for(size_t x = 0; x < m_width; x++)
        {
            *byteArray++ = static_cast<uint8_t>(*(*pptr++));
        }
        pptr += m_alignedWidth - m_width;
    }
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline typename Frame<MIN_N, MAX_N>::RegionIterator Frame<MIN_N, MAX_N>::begin()
{
    return RegionIterator(&m_data[0], &m_ptrs[0], 0, 0, 0,
                          m_width, m_height, m_alignedWidth - m_width);
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline typename Frame<MIN_N, MAX_N>::RegionIterator Frame<MIN_N, MAX_N>::end()
{
    return RegionIterator();
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline typename Frame<MIN_N, MAX_N>::RegionIterator
    Frame<MIN_N, MAX_N>::regionBegin(coord_t xstart, coord_t ystart, coord_t width, coord_t height)
{
    return RegionIterator(m_ptrs[ystart * m_alignedWidth + xstart],
                          &m_ptrs[ystart * m_alignedWidth + xstart], xstart, ystart,
                          xstart, xstart + width, ystart + height, m_alignedWidth - width);
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline typename Frame<MIN_N, MAX_N>::RegionIterator
    Frame<MIN_N, MAX_N>::regionEnd()
{
    return RegionIterator();
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline typename Frame<MIN_N, MAX_N>::HorizontalBlockIterator Frame<MIN_N, MAX_N>::horizontalBegin(coord_t block)
{
    return HorizontalBlockIterator(&m_data[0] + block * MIN_N * MIN_N);
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline typename Frame<MIN_N, MAX_N>::HorizontalBlockIterator Frame<MIN_N, MAX_N>::horizontalEnd()
{
    return HorizontalBlockIterator(&m_data[0] + m_data.size());
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline typename Frame<MIN_N, MAX_N>::VerticalBlockIterator Frame<MIN_N, MAX_N>::verticalBegin(coord_t block)
{
    if(block == m_data.size() / (MIN_N * MIN_N))
    {
        return VerticalBlockIterator();
    }
    return VerticalBlockIterator(&m_data[0] + block * MIN_N * MIN_N, m_data.size() / (MIN_N * MIN_N) - block);
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline typename Frame<MIN_N, MAX_N>::VerticalBlockIterator Frame<MIN_N, MAX_N>::verticalEnd()
{
    return VerticalBlockIterator();
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline typename Frame<MIN_N, MAX_N>::ScanningBlockIterator
    Frame<MIN_N, MAX_N>::scanningBegin(const coord_t *scan, coord_t block)
{
    if(block == m_data.size() / (MIN_N * MIN_N))
    {
        return ScanningBlockIterator();
    }
    return ScanningBlockIterator(&m_data[0] + block * MIN_N * MIN_N, m_data.size() / (MIN_N * MIN_N) - block, scan);
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline typename Frame<MIN_N, MAX_N>::ScanningBlockIterator Frame<MIN_N, MAX_N>::scanningEnd()
{
    return ScanningBlockIterator();
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline void swap(Frame<MIN_N, MAX_N> &first, Frame<MIN_N, MAX_N> &second)
{
    using std::swap;
    swap(first.m_width, second.m_width);
    swap(first.m_height, second.m_height);
    swap(first.m_alignedWidth, second.m_alignedWidth);
    swap(first.m_alignedHeight, second.m_alignedHeight);
    swap(first.m_data, second.m_data);
    swap(first.m_ptrs, second.m_ptrs);
}

template <uint32_t MIN_N, uint32_t MAX_N>
template <typename T>
inline bool Frame<MIN_N, MAX_N>::BaseIterator<T>::operator==(const T& other) const
{
    return m_ptr == other.m_ptr;
}

template <uint32_t MIN_N, uint32_t MAX_N>
template <typename T>
inline bool Frame<MIN_N, MAX_N>::BaseIterator<T>::operator!=(const T& other) const
{
    return !(*this == other);
}

template <uint32_t MIN_N, uint32_t MAX_N>
template <typename T>
inline typename Frame<MIN_N, MAX_N>::template BaseIterator<T>::reference
    Frame<MIN_N, MAX_N>::BaseIterator<T>::operator*()
{
    return *m_ptr;
}

template <uint32_t MIN_N, uint32_t MAX_N>
template <typename T>
inline T &Frame<MIN_N, MAX_N>::BaseIterator<T>::operator++()
{
    static_cast<T *>(this)->increment();
    return *static_cast<T *>(this);
}

template <uint32_t MIN_N, uint32_t MAX_N>
template <typename T>
inline T Frame<MIN_N, MAX_N>::BaseIterator<T>::operator++(int)
{
    T result(*static_cast<T *>(this));
    static_cast<T *>(this)->increment();
    return result;
}

template <uint32_t MIN_N, uint32_t MAX_N>
template <typename T>
inline Frame<MIN_N, MAX_N>::BaseIterator<T>::BaseIterator(data_t *ptr)
    : m_ptr(ptr)
{}

template <uint32_t MIN_N, uint32_t MAX_N>
inline Frame<MIN_N, MAX_N>::RegionIterator::RegionIterator(data_t *ptr, data_t **pptr,
                                                           coord_t x, coord_t y, coord_t xstart,
                                                           coord_t xend, coord_t yend, coord_t skip)
    : BaseIterator<RegionIterator>(ptr), m_pptr(pptr), m_x(x), m_y(y),
      m_xstart(xstart), m_xend(xend), m_yend(yend), m_skip(skip)
{}

template <uint32_t MIN_N, uint32_t MAX_N>
inline void Frame<MIN_N, MAX_N>::RegionIterator::increment()
{
    m_x++;
    if(m_x == m_xend)
    {
        m_x = m_xstart;
        m_y++;
        if(m_y == m_yend)
        {
            this->m_ptr = NULL;
            return;
        }
        m_pptr += m_skip + 1;
    }
    else
    {
        m_pptr++;
    }
    this->m_ptr = *m_pptr;
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline Frame<MIN_N, MAX_N>::HorizontalBlockIterator::HorizontalBlockIterator(data_t *ptr)
    : BaseIterator<HorizontalBlockIterator>(ptr)
{}

template <uint32_t MIN_N, uint32_t MAX_N>
inline void Frame<MIN_N, MAX_N>::HorizontalBlockIterator::increment()
{
    this->m_ptr++;
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline Frame<MIN_N, MAX_N>::VerticalBlockIterator::VerticalBlockIterator(data_t *ptr, coord_t count)
    : BaseIterator<VerticalBlockIterator>(ptr), m_origin(ptr), m_x(0), m_y(0), m_block(0), m_count(count)
{}

template <uint32_t MIN_N, uint32_t MAX_N>
inline void Frame<MIN_N, MAX_N>::VerticalBlockIterator::increment()
{
    m_y++;
    if(m_y == MIN_N)
    {
        m_y = 0;
        m_x++;
        if(m_x == MIN_N)
        {
            m_x = 0;
            m_block++;
            if(m_block == m_count)
            {
                this->m_ptr = NULL;
            }
            else
            {
                this->m_ptr++;
                m_origin = this->m_ptr;
            }
        }
        else
        {
            m_origin++;
            this->m_ptr = m_origin;
        }
    }
    else
    {
        this->m_ptr += MIN_N;
    }
}

template <uint32_t MIN_N, uint32_t MAX_N>
inline Frame<MIN_N, MAX_N>::ScanningBlockIterator::ScanningBlockIterator(data_t *ptr, coord_t count,
                                                                         const coord_t *scan)
    : BaseIterator<ScanningBlockIterator>(ptr), m_origin(ptr), m_block(0), m_count(count),
      m_scan(scan), m_scanPtr(scan)
{}

template <uint32_t MIN_N, uint32_t MAX_N>
inline void Frame<MIN_N, MAX_N>::ScanningBlockIterator::increment()
{
    m_scanPtr++;
    if(m_scanPtr - m_scan == MIN_N * MIN_N)
    {
        m_scanPtr = m_scan;
        m_block++;
        if(m_block == m_count)
        {
            this->m_ptr = NULL;
        }
        else
        {
            m_origin += MIN_N * MIN_N;
            this->m_ptr = m_origin;
        }
    }
    else
    {
        this->m_ptr = m_origin + *m_scanPtr;
    }
}

}
