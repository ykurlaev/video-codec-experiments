#include <iterator>
#include <stdexcept>

namespace Codec
{

inline Precompressor::Precompressor()
    : m_byteArray(NULL), m_bytesProcessed(0)
{}

inline void Precompressor::setByteArray(uint8_t *byteArray)
{
    m_byteArray = byteArray;
    m_bytesProcessed = 0;
}

inline size_t Precompressor::getBytesProcessed() const
{
    return m_bytesProcessed;
}

template <typename Iterator>
inline void Precompressor::applyForward(Iterator begin, Iterator end)
{
    if(m_byteArray == NULL)
    {
        return;
    }
    uint8_t *byteArrayOriginal = m_byteArray;
    uint8_t zeros = 0;
    for(; begin != end; ++begin)
    {
        typedef typename std::iterator_traits<Iterator>::value_type value_type;
        typedef typename MakeUnsigned<value_type>::t u_value_type;
        u_value_type uValue = static_cast<u_value_type>(*begin);
        uint8_t bytes[sizeof(u_value_type)];
        for(size_t b = 0; b < sizeof(u_value_type); b++)
        {
            bytes[b] = static_cast<uint8_t>((uValue >> ((sizeof(bytes) - 1 - b) * 8)) & 0xFF);
        }
        if(uValue == 0)
        {
            if(zeros == 0xFF)
            {
                *m_byteArray++ = 0;
                *m_byteArray++ = zeros;
                zeros = 0;
            }
            zeros++;
        }
        else
        {
            if(zeros != 0)
            {
                *m_byteArray++ = 0;
                *m_byteArray++ = zeros;
                zeros = 0;
            }
            if((void)0, sizeof(u_value_type) == 1)
            {
                *m_byteArray++ = static_cast<uint8_t>(uValue);
            }
            else
            {
                uint8_t sign = bytes[0] >> 7 != 0 ? 0xFF : 0;
                size_t byteCount = sizeof(u_value_type) + 1;
                for(size_t b = 0; b < sizeof(u_value_type); b++)
                {
                    uint8_t f = (b != 0) ? bytes[b - 1] : sign;
                    uint8_t s = bytes[b];
                    uint8_t u = ((f << (sizeof(u_value_type) - b + 1)) |
                                 (s >> (8 - (sizeof(u_value_type) - b + 1)))) & 0xFF;
                    if(u != sign)
                    //if((bytes[b] >> (sizeof(u_value_type) - b))
                    //   != (sign >> (sizeof(u_value_type) - b)))
                    {
                        break;
                    }
                    byteCount--;
                }
                size_t b = 0;
                if(byteCount > sizeof(u_value_type))
                {
                    *m_byteArray++ = ((0x00FF >> (9 - byteCount)) << (9 - byteCount)) |
                                     (sign & (0x00FF >> byteCount));
                }
                else
                {
                    *m_byteArray++ = ((0x00FF >> (9 - byteCount)) << (9 - byteCount)) |
                                     (bytes[sizeof(u_value_type) - byteCount] & (0x00FF >> byteCount));
                    b++;
                }
                for(; b < byteCount; b++)
                {
                    *m_byteArray++ = bytes[sizeof(u_value_type) - byteCount + b];
                }
            }
        }
    }
    if(zeros != 0)
    {
        *m_byteArray++ = 0;
        *m_byteArray++ = zeros;
    }
    m_bytesProcessed += m_byteArray - byteArrayOriginal;
}

template <typename Iterator>
inline void Precompressor::applyReverse(Iterator begin, Iterator end)
{
    if(m_byteArray == NULL)
    {
        return;
    }
    uint8_t *byteArrayOriginal = m_byteArray;
    while(begin != end)
    {
        typedef typename std::iterator_traits<Iterator>::value_type value_type;
        typedef typename MakeUnsigned<value_type>::t u_value_type;
        if(*m_byteArray == 0)
        {
            m_byteArray++;
            uint8_t zeros = *m_byteArray++;
            for(uint8_t i = 0; i < zeros && begin != end; i++)
            {
                *begin = 0;
                ++begin;
            }
        }
        else
        {
            if((void)0, sizeof(u_value_type) == 1)
            {
                *begin = *m_byteArray++;
            }
            else
            {
                uint8_t bytes[sizeof(u_value_type)];
                uint8_t firstByte = *m_byteArray++;
                size_t byteCount = 1;
                for(; ; byteCount++)
                {
                    if(((firstByte >> (8 - byteCount)) & 0x01) == 0)
                    {
                        break;
                    }
                }
                uint8_t sign = (((firstByte >> (8 - byteCount - 1)) & 0x01) == 1) ? 0xFF : 0;
                for(size_t b = 0; b < sizeof(u_value_type) - byteCount; b++)
                {
                    bytes[b] = sign;
                }
                size_t b = 0;
                if(byteCount <= sizeof(u_value_type))
                {
                    bytes[sizeof(u_value_type) - byteCount] = ((sign >> (8 - byteCount)) << (8 - byteCount))
                                                              | (firstByte & (0x00FF >> byteCount));
                    b++;
                }
                for(; b < byteCount; b++)
                {
                    bytes[sizeof(u_value_type) - byteCount + b] = *m_byteArray++;
                }
                u_value_type uValue = 0;
                for(size_t b = 0; b < sizeof(u_value_type); b++)
                {
                    uValue |= bytes[b] << ((sizeof(u_value_type) - 1 - b) * 8);
                }
                *begin = static_cast<value_type>(uValue);
            }
            ++begin;
        }
    }
    m_bytesProcessed += m_byteArray - byteArrayOriginal;
}

}