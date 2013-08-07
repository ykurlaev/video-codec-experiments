#include <algorithm>
#include <cassert>
#include <cstring>

namespace Codec
{

inline RegionBuffer::RegionBuffer(coord_t width, coord_t height, coord_t length)
    : m_width(width), m_height(height), m_length(length),
      m_data(width * height * length)
{}

inline RegionBuffer::RegionBuffer(const RegionBuffer &other)
    : m_width(other.m_width), m_height(other.m_height),
      m_length(other.m_length), m_data(other.m_data)
{}

inline RegionBuffer &RegionBuffer::operator=(const RegionBuffer &other)
{
    RegionBuffer tmp(other);
    swap(*this, tmp);
    return *this;
}

inline RegionBuffer::coord_t RegionBuffer::getWidth() const
{
    return m_width;
}

inline RegionBuffer::coord_t RegionBuffer::getHeight() const
{
    return m_height;
}

inline RegionBuffer::coord_t RegionBuffer::getLength() const
{
    return m_length;
}

inline const RegionBuffer::data_t &RegionBuffer::operator()(coord_t x, coord_t y, coord_t t) const
{
    assert(x < m_width && y < m_height && t < m_length);
    return m_data[(t * m_height + y) * m_width + x];
}

inline RegionBuffer::data_t &RegionBuffer::operator()(coord_t x, coord_t y, coord_t t)
{
    assert(x < m_width && y < m_height && t < m_length);
    return m_data[(t * m_height + y) * m_width + x];
}

inline void RegionBuffer::clear()
{
    memset(&m_data[0], 0, m_width * m_height * m_length * sizeof(data_t));
}

inline void swap(RegionBuffer &first, RegionBuffer &second)
{
    using std::swap;
    swap(first.m_width, second.m_width);
    swap(first.m_height, second.m_height);
    swap(first.m_length, second.m_length);
    swap(first.m_data, second.m_data);
}

}