#include <algorithm>
#include <cstring>

namespace Codec
{

template <uint32_t N>
inline Frame<N>::Frame(coord_t width, coord_t height, coord_t alignment)
    : m_width(width), m_height(height),
      m_alignedWidth(((width + alignment - 1) / alignment) * alignment),
      m_alignedHeight(((height + alignment - 1) / alignment) * alignment),
      m_data(m_alignedWidth * m_alignedHeight)
{}

template <uint32_t N>
inline Frame<N>::Frame(const Frame &other)
    : m_width(other.m_width), m_height(other.m_height),
      m_alignedWidth(other.m_alignedWidth), m_alignedHeight(other.m_alignedHeight),
      m_data(other.m_data)
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
inline typename Frame<N>::Iterator Frame<N>::end() const
{
    return Iterator();
}

template <uint32_t N>
template <typename F>
inline void Frame<N>::apply(F &f)
{
    applyHorizontal(f);
}

template <uint32_t N>
template <typename F>
inline void Frame<N>::applyHorizontal(F &f)
{
    f(HorizontalIterator(&m_data[0]), HorizontalIterator(&m_data[0] + m_data.size()));
}

template <uint32_t N>
template <typename F>
inline void Frame<N>::applyVertical(F &f)
{
    f(VerticalIterator(&m_data[0], m_data.size() / (BLOCK_SIZE * BLOCK_SIZE)), VerticalIterator());
}

template <uint32_t N>
template <typename F>
inline void Frame<N>::applyScanning(F &f, const coord_t *scan)
{
    f(ScanningIterator(&m_data[0], m_data.size() / (BLOCK_SIZE * BLOCK_SIZE), scan), ScanningIterator());
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
}

template <uint32_t N>
inline bool Frame<N>::BaseIterator::operator==(const BaseIterator& other) const
{
    return m_ptr == other.m_ptr;
}

template <uint32_t N>
inline bool Frame<N>::BaseIterator::operator!=(const BaseIterator& other) const
{
    return !(*this == other);
}

template <uint32_t N>
inline typename Frame<N>::BaseIterator::reference Frame<N>::BaseIterator::operator*()
{
    return *m_ptr;
}

template <uint32_t N>
inline Frame<N>::BaseIterator::BaseIterator(data_t *ptr)
    : m_ptr(ptr)
{}

template <uint32_t N>
inline Frame<N>::Iterator::Iterator()
    : BaseIterator(NULL), m_origin(NULL), m_x(0), m_y(0), 
      m_width(0), m_height(0), m_skip(0)
{}

template <uint32_t N>
inline Frame<N>::Iterator::Iterator(Frame &frame)
    : BaseIterator(&frame.m_data[0]), m_origin(&frame.m_data[0]), m_x(0), m_y(0), 
      m_width(frame.m_width), m_height(frame.m_height), m_skip(frame.m_alignedWidth - frame.m_width)
{}

template <uint32_t N>
inline typename Frame<N>::Iterator &Frame<N>::Iterator::operator++()
{
    this->m_ptr++;
    m_x++;
    if(m_x == m_width)
    {
        m_x = 0;
        m_y++;
        if(m_y == m_height)
        {
            this->m_ptr = NULL;
        }
        else if(m_y % N == 0)
        {
            this->m_ptr += m_skip;
            m_origin = this->m_ptr;
        }
        else
        {
            m_origin += N;
            this->m_ptr = m_origin;
        }
    }
    else if(m_x % N == 0)
    {
        this->m_ptr += N * (N - 1);
    }
    return *this;
}

template <uint32_t N>
inline typename Frame<N>::Iterator Frame<N>::Iterator::operator++(int)
{
    Iterator result(*this);
    ++(*this);
    return result;
}

template <uint32_t N>
inline Frame<N>::HorizontalIterator::HorizontalIterator()
    : BaseIterator(NULL)
{}

template <uint32_t N>
inline Frame<N>::HorizontalIterator::HorizontalIterator(data_t *ptr)
    : BaseIterator(ptr)
{}

template <uint32_t N>
inline typename Frame<N>::HorizontalIterator &Frame<N>::HorizontalIterator::operator++()
{
    this->m_ptr++;
    return *this;
}

template <uint32_t N>
inline typename Frame<N>::HorizontalIterator Frame<N>::HorizontalIterator::operator++(int)
{
    HorizontalIterator result(*this);
    ++(*this);
    return result;
}

template <uint32_t N>
inline Frame<N>::VerticalIterator::VerticalIterator()
    : BaseIterator(NULL), m_origin(NULL), m_x(0), m_y(0), m_block(0), m_count(0)
{}


template <uint32_t N>
inline Frame<N>::VerticalIterator::VerticalIterator(data_t *ptr, coord_t count)
    : BaseIterator(ptr), m_origin(ptr), m_x(0), m_y(0), m_block(0), m_count(count)
{}

template <uint32_t N>
inline typename Frame<N>::VerticalIterator &Frame<N>::VerticalIterator::operator++()
{
    m_y++;
    if(m_y % N == 0)
    {
        m_y = 0;
        m_x++;
        if(m_x % N == 0)
        {
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
    return *this;
}

template <uint32_t N>
inline typename Frame<N>::VerticalIterator Frame<N>::VerticalIterator::operator++(int)
{
    VerticalIterator result(*this);
    ++(*this);
    return result;
}

template <uint32_t N>
inline Frame<N>::ScanningIterator::ScanningIterator()
    : BaseIterator(NULL), m_origin(NULL), m_block(0), m_count(0),
      m_scan(NULL), m_scanPtr(NULL)
{}

template <uint32_t N>
inline Frame<N>::ScanningIterator::ScanningIterator(data_t *ptr, coord_t count, const coord_t *scan)
    : BaseIterator(ptr), m_origin(ptr), m_block(0), m_count(count),
      m_scan(scan), m_scanPtr(scan)
{}

template <uint32_t N>
inline typename Frame<N>::ScanningIterator &Frame<N>::ScanningIterator::operator++()
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
    return *this;
}

template <uint32_t N>
inline typename Frame<N>::ScanningIterator Frame<N>::ScanningIterator::operator++(int)
{
    ScanningIterator result(*this);
    ++(*this);
    return result;
}

}