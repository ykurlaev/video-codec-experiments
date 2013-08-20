#include <algorithm>
#include <cstring>

namespace Codec
{

template <uint32_t N>
inline Frame<N>::Frame(coord_t width, coord_t height, coord_t alignment)
    : m_width(width), m_height(height),
      m_alignedWidth(((width + alignment - 1) / alignment) * alignment),
      m_alignedHeight(((height + alignment - 1) / alignment) * alignment),
      m_data(m_alignedWidth * m_alignedHeight), m_ptrs(m_alignedWidth * m_alignedHeight)
{
    coord_t xCount = m_alignedWidth / N;
    coord_t xBlock = 0, yBlock = 0, i = 0;
    for(int *ptr = &m_data[0]; ptr != &m_data[0] + m_data.size(); ptr++)
    {
        coord_t y = i / N, x = i % N;
        m_ptrs[(yBlock * N + y) * m_alignedWidth + xBlock * N + x] = ptr;
        i++;
        if(i == N * N)
        {
            i = 0;
            xBlock++;
            if(xBlock == xCount)
            {
                xBlock = 0;
                yBlock++;
            }
        }
    }
}

template <uint32_t N>
inline Frame<N>::Frame(const Frame &other)
    : m_width(other.m_width), m_height(other.m_height),
      m_alignedWidth(other.m_alignedWidth), m_alignedHeight(other.m_alignedHeight),
      m_data(other.m_data), m_ptrs(other.m_ptrs)
{}

template <uint32_t N>
inline Frame<N> &Frame<N>::operator=(const Frame &other)
{
    Frame tmp(other);
    swap(*this, tmp);
    return *this;
}

template <uint32_t N>
inline typename Frame<N>::coord_t Frame<N>::getWidth() const 
{
    return m_width;
}

template <uint32_t N>
inline typename Frame<N>::coord_t Frame<N>::getHeight() const
{
    return m_height;
}

template <uint32_t N>
inline typename Frame<N>::coord_t Frame<N>::getAlignedWidth() const
{
    return m_alignedWidth;
}

template <uint32_t N>
inline typename Frame<N>::coord_t Frame<N>::getAlignedHeight() const
{
    return m_alignedHeight;
}

template <uint32_t N>
inline void Frame<N>::clear()
{
    memset(&m_data[0], 0, m_data.size() * sizeof(data_t));
}

template <uint32_t N>
inline typename Frame<N>::Iterator Frame<N>::begin()
{
    return Iterator(*this);
}

template <uint32_t N>
inline typename Frame<N>::Iterator Frame<N>::end()
{
    return Iterator();
}

template <uint32_t N>
inline typename Frame<N>::HorizontalIterator Frame<N>::horizontalBegin()
{
    return HorizontalIterator(&m_data[0]);
}

template <uint32_t N>
inline typename Frame<N>::HorizontalIterator Frame<N>::horizontalEnd()
{
    return HorizontalIterator(&m_data[0] + m_data.size());
}

template <uint32_t N>
inline typename Frame<N>::VerticalIterator Frame<N>::verticalBegin()
{
    return VerticalIterator(&m_data[0], m_data.size() / (BLOCK_SIZE * BLOCK_SIZE));
}

template <uint32_t N>
inline typename Frame<N>::VerticalIterator Frame<N>::verticalEnd()
{
    return VerticalIterator();
}

template <uint32_t N>
inline typename Frame<N>::ScanningIterator Frame<N>::scanningBegin(const coord_t *scan)
{
    return ScanningIterator(&m_data[0], m_data.size() / (BLOCK_SIZE * BLOCK_SIZE), scan);
}

template <uint32_t N>
inline typename Frame<N>::ScanningIterator Frame<N>::scanningEnd()
{
    return ScanningIterator();
}

template <uint32_t N>
inline void swap(Frame<N> &first, Frame<N> &second)
{
    using std::swap;
    swap(first.m_width, second.m_width);
    swap(first.m_height, second.m_height);
    swap(first.m_alignedWidth, second.m_alignedWidth);
    swap(first.m_alignedHeight, second.m_alignedHeight);
    swap(first.m_data, second.m_data);
    swap(first.m_ptrs, second.m_ptrs);
}

template <uint32_t N>
template <typename T>
inline bool Frame<N>::BaseIterator<T>::operator==(const BaseIterator& other) const
{
    return m_ptr == other.m_ptr;
}

template <uint32_t N>
template <typename T>
inline bool Frame<N>::BaseIterator<T>::operator!=(const BaseIterator& other) const
{
    return !(*this == other);
}

template <uint32_t N>
template <typename T>
inline typename Frame<N>::template BaseIterator<T>::reference Frame<N>::BaseIterator<T>::operator*()
{
    return *m_ptr;
}

template <uint32_t N>
template <typename T>
inline T &Frame<N>::BaseIterator<T>::operator++()
{
    static_cast<T *>(this)->increment();
    return *static_cast<T *>(this);
}

template <uint32_t N>
template <typename T>
inline T Frame<N>::BaseIterator<T>::operator++(int)
{
    T result(*static_cast<T *>(this));
    static_cast<T *>(this)->increment();
    return result;
}

template <uint32_t N>
template <typename T>
inline Frame<N>::BaseIterator<T>::BaseIterator(data_t *ptr)
    : m_ptr(ptr)
{}

template <uint32_t N>
inline Frame<N>::Iterator::Iterator()
    : BaseIterator<Iterator>(NULL), m_pptr(NULL), m_x(0), m_y(0),
      m_width(0), m_height(0), m_skip(0)
{}

template <uint32_t N>
inline Frame<N>::Iterator::Iterator(Frame &frame)
    : BaseIterator<Iterator>(&frame.m_data[0]), m_pptr(&frame.m_ptrs[0]), m_x(0), m_y(0),
      m_width(frame.m_width), m_height(frame.m_height), m_skip(frame.m_alignedWidth - frame.m_width)
{}

template <uint32_t N>
inline void Frame<N>::Iterator::increment()
{
    m_x++;
    if(m_x == m_width)
    {
        m_x = 0;
        m_y++;
        if(m_y == m_height)
        {
            this->m_ptr = NULL;
            return;
        }
        m_pptr += m_skip;
    }
    else
    {
        m_pptr++;
    }
    this->m_ptr = *m_pptr;
}

template <uint32_t N>
inline Frame<N>::HorizontalIterator::HorizontalIterator()
    : BaseIterator<HorizontalIterator>(NULL)
{}

template <uint32_t N>
inline Frame<N>::HorizontalIterator::HorizontalIterator(data_t *ptr)
    : BaseIterator<HorizontalIterator>(ptr)
{}

template <uint32_t N>
inline void Frame<N>::HorizontalIterator::increment()
{
    this->m_ptr++;
}

template <uint32_t N>
inline Frame<N>::VerticalIterator::VerticalIterator()
    : BaseIterator<VerticalIterator>(NULL), m_origin(NULL), m_x(0), m_y(0), m_block(0), m_count(0)
{}


template <uint32_t N>
inline Frame<N>::VerticalIterator::VerticalIterator(data_t *ptr, coord_t count)
    : BaseIterator<VerticalIterator>(ptr), m_origin(ptr), m_x(0), m_y(0), m_block(0), m_count(count)
{}

template <uint32_t N>
inline void Frame<N>::VerticalIterator::increment()
{
    m_y++;
    if(m_y == N)
    {
        m_y = 0;
        m_x++;
        if(m_x == N)
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
        this->m_ptr += N;
    }
}

template <uint32_t N>
inline Frame<N>::ScanningIterator::ScanningIterator()
    : BaseIterator<ScanningIterator>(NULL), m_origin(NULL), m_block(0), m_count(0),
      m_scan(NULL), m_scanPtr(NULL)
{}

template <uint32_t N>
inline Frame<N>::ScanningIterator::ScanningIterator(data_t *ptr, coord_t count, const coord_t *scan)
    : BaseIterator<ScanningIterator>(ptr), m_origin(ptr), m_block(0), m_count(count),
      m_scan(scan), m_scanPtr(scan)
{}

template <uint32_t N>
inline void Frame<N>::ScanningIterator::increment()
{
    m_scanPtr++;
    if(m_scanPtr - m_scan == N * N)
    {
        m_scanPtr = m_scan;
        m_block++;
        if(m_block == m_count)
        {
            this->m_ptr = NULL;
        }
        else
        {
            m_origin += N * N;
            this->m_ptr = m_origin;
        }
    }
    else
    {
        this->m_ptr = m_origin + *m_scanPtr;
    }
}

}