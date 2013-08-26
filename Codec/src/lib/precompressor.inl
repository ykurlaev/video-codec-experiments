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
        typename MakeUnsigned<typename Iterator::value_type>::t u = *begin;
        uint8_t lsb = u & 0xFF;
        uint8_t msb = (u >> 8) & 0xFF;
        if(msb == 0 && lsb == 0)
        {
            if(zeros == 0x3F)
            {
                *m_byteArray++ = 0xC0 | zeros;
                zeros = 0;
            }
            zeros++;
        }
        else
        {
            if(zeros != 0)
            {
                *m_byteArray++ = 0xC0 | zeros;
                zeros = 0;
            }
            bool sign = msb >> 7 == 1;
            if(msb == (sign ? ~0 : 0) && lsb >> 6 == (sign ? 0x3 : 0))
            {
                *m_byteArray++ = lsb & 0x7F;
            }
            else
            {
                *m_byteArray++ = (msb & 0x3F) | 0x80;
                *m_byteArray++ = lsb;
            }
        }
    }
    if(zeros != 0)
    {
        *m_byteArray++ = 0xC0 | zeros;
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
        if(*m_byteArray >> 6 == 0x3)
        {
            uint8_t zeros = *m_byteArray++ & 0x3F;
            for(uint8_t j = 0; j < zeros && begin != end; j++)
            {
                *begin = 0;
                ++begin;
            }
        }
        else
        {
            uint8_t msb;
            uint8_t lsb;
            if(*m_byteArray >> 7 == 0)
            {
                uint8_t sign = (*m_byteArray >> 6 == 1) ? ~0 : 0;
                msb = sign;
                lsb = (*m_byteArray++ & 0x7F) | (sign & 0x80);
            }
            else
            {
                msb = (*m_byteArray & 0x3F) | (((*m_byteArray & 0x20) == 0) ? 0 : 0xC0);
                m_byteArray++;
                lsb = *m_byteArray++;
            }
            *begin = static_cast<int16_t>(msb << 8 | lsb);
            ++begin;
        }
    }
    m_bytesProcessed += m_byteArray - byteArrayOriginal;
}

}