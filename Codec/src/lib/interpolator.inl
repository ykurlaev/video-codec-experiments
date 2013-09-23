#include <algorithm>

namespace Codec
{

inline Interpolator::Interpolator(coord_t width, coord_t height, int scale)
    : m_width(width), m_height(height), m_scale(scale),
      m_source(width * height), m_interpolated((width * scale - 1) * (height * scale - 1))
{ }

template <typename Iter>
inline void Interpolator::fillFrom(Iter begin, Iter end)
{
    std::copy(begin, end, &m_source[0]);
    data_t *src = &m_source[0];
    data_t *dst = &m_interpolated[0];
    for(coord_t y = 0; y < m_height; y++)
    {
        for(coord_t x = 0; x < m_width - 1; x++)
        {
            *dst++ = *src++;
            *dst++ = (*(src - 1) + *(src + 1)) / 2;
        }
        *dst++ = *src++;
        dst += m_width * m_scale;
    }
    dst = &m_interpolated[0];
    for(coord_t y = 0; y < m_height - 1; y++)
    {
        dst += m_width * m_scale;
        for(coord_t x = 0; x < m_width * m_scale; x++)
        {
            *dst++ = (*(dst - m_width * m_scale) + *(dst + m_width * m_scale)) / 2;
        }
    }
}

inline Interpolator::Iterator Interpolator::regionBegin(coord_t x, coord_t y,
                                                        scoord_t xOffset, scoord_t yOffset,
                                                        coord_t width, coord_t height)
{
    coord_t realX = x * m_scale + xOffset;
    coord_t realY = y * m_scale + yOffset;
    realX = realX < 0 ? 0 : (realX > m_width * m_scale - 1 ? m_width * m_scale - 1 : realX);
    realY = realY < 0 ? 0 : (realY > m_height * m_scale - 1 ? m_height * m_scale - 1 : realY);
    return Iterator(&m_interpolated[0]/* + realY * (m_width * m_scale - 1) + realX*/, realX, realY,
                    m_width * m_scale - 1, m_height * m_scale - 1, realX, realX + width * m_scale - 1,
                    realY + height * m_scale - 1, m_width * m_scale - width * m_scale, m_scale);
}

inline Interpolator::Iterator Interpolator::regionEnd()
{
    return Iterator();
}

inline Interpolator::Iterator::Iterator(data_t *ptr, scoord_t x, scoord_t y, coord_t width, coord_t height,
                                        scoord_t xstart, scoord_t xend, scoord_t yend, coord_t skip, int scale)
    : m_ptr(ptr), m_x(x), m_y(y), m_width(width), m_height(height),
      m_xstart(xstart), m_xend(xend), m_yend(yend), m_skip(skip), m_scale(scale)
{ }

inline bool Interpolator::Iterator::operator==(const Iterator& other) const
{
    return m_ptr == other.m_ptr;
}

inline bool Interpolator::Iterator::operator!=(const Iterator& other) const
{
    return !(*this == other);
}

inline Interpolator::data_t &Interpolator::Iterator::operator*()
{
    scoord_t x = m_x < 0 ? 0 : (m_x > static_cast<scoord_t>(m_width) - 1 ? m_width - 1 : m_x);
    scoord_t y = m_y < 0 ? 0 : (m_y > static_cast<scoord_t>(m_width) - 1 ? m_height - 1 : m_y);
    return *(m_ptr + m_width * y + x);
}

inline Interpolator::Iterator &Interpolator::Iterator::operator++()
{
    m_x += m_scale;
    if(m_x == m_xend)
    {
        m_x = m_xstart;
        m_y += m_scale;
        if(m_y == m_yend)
        {
            this->m_ptr = NULL;
            return *this;
        }
        //if(m_x >= 0 && m_y >= 0 &&
        //   m_x < static_cast<scoord_t>(m_width) && m_y < static_cast<scoord_t>(m_height))
        //{
        //    m_ptr += m_skip;
        //}
    }
    else
    {
        //if(m_x >= 0 && m_y >= 0 &&
        //   m_x < static_cast<scoord_t>(m_width) && m_y < static_cast<scoord_t>(m_height))
        //{
        //    m_ptr += m_scale;
        //}
    }
    return *this;
}

inline Interpolator::Iterator Interpolator::Iterator::operator++(int)
{
    Iterator result(*this);
    ++(*this);
    return result;
}

}
