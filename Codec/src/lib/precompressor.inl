namespace Codec
{

inline Precompressor::Precompressor(uint8_t *byteArray)
    : m_byteArray(byteArray)
{}

inline void Precompressor::setByteArray(uint8_t *byteArray)
{
    m_byteArray = byteArray;
}

template <typename Iterator>
inline size_t Precompressor::applyForward(Iterator begin, Iterator end)
{
    if(m_byteArray == NULL)
    {
        return 0;
    }
    size_t result = 0;
    uint8_t zeros = 0;
    for(; begin != end; ++begin)
    {
        typename MakeUnsigned<typename Iterator::value_type>::t u = *begin;
        uint8_t lsb = u & 0xFF;
        uint8_t msb = (u >> 8) & 0xFF;
        if(msb == 0 && lsb == 0 && zeros != 0x3F)
        {
            zeros++;
        }
        else
        {
            if(zeros != 0)
            {
                m_byteArray[result] = 0xC0 | zeros;
                result++;
                zeros = 0;
            }
            bool sign = msb >> 7 == 1;
            if(msb == (sign ? ~0 : 0) && lsb >> 6 == (sign ? 0x3 : 0))
            {
                m_byteArray[result] = lsb & 0x7F;
                result++;
            }
            else
            {
                m_byteArray[result] = (msb & 0x3F) | 0x80;
                m_byteArray[result + 1] = lsb;
                result += 2;
            }
        }
    }
    return result;
}

template <typename Iterator>
inline size_t Precompressor::applyReverse(Iterator begin, Iterator end)
{
    if(m_byteArray == NULL)
    {
        return 0;
    }
    size_t result = 0;
    size_t i = 0;
    while(begin != end)
    {
        if(m_byteArray[i] >> 6 == 0x3)
        {
            uint8_t zeros = m_byteArray[i] & 0x3F;
            for(uint8_t j = 0; j < zeros && begin != end; j++)
            {
                *begin = 0;
                ++begin;
                result++;
            }
        }
        else
        {
            uint8_t msb;
            uint8_t lsb;
            if(m_byteArray[i] >> 7 == 0)
            {
                uint8_t sign = (m_byteArray[i] >> 6 == 1) ? ~0 : 0;
                msb = sign;
                lsb = (m_byteArray[i] & 0x7F) | (sign & 0x80);
            }
            else
            {
                msb = (m_byteArray[i] & 0x3F) | (((m_byteArray[i] & 0x20) == 0) ? 0 : 0xC0);
                i++;
                lsb = m_byteArray[i];
            }
            *begin = static_cast<int16_t>(msb << 8 | lsb);
            ++begin;
            result++;
        }
        i++;
    }
    return result;
}

}