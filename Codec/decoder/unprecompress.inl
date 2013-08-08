namespace Codec
{

inline UnPrecompress::UnPrecompress(uint8_t *input)
    : m_input(input), m_outputSize(0)
{}

inline void UnPrecompress::setInput(uint8_t *input)
{
    m_input = input;
}

inline size_t UnPrecompress::getOutputSize() const
{
    return m_outputSize;
}

template <typename Iterator>
inline void UnPrecompress::operator()(Iterator begin, Iterator end)
{
    if(m_input == NULL)
    {
        return;
    }
    m_outputSize = 0;
    size_t i = 0;
    while(begin != end)
    {
        if(m_input[i] >> 6 == 0x3)
        {
            uint8_t zeros = m_input[i] & 0x3F;
            for(uint8_t j = 0; j < zeros && begin != end; j++)
            {
                *begin = 0;
                ++begin;
                m_outputSize++;
            }
        }
        else
        {
            uint8_t msb;
            uint8_t lsb;
            if(m_input[i] >> 7 == 0)
            {
                uint8_t sign = (m_input[i] >> 6 == 1) ? ~0 : 0;
                msb = sign;
                lsb = (m_input[i] & 0x7F) | (sign & 0x80);
            }
            else
            {
                msb = (m_input[i] & 0x3F) | (((m_input[i] & 0x20) == 0) ? 0 : 0xC0);
                i++;
                lsb = m_input[i];
            }
            *begin = static_cast<int16_t>(msb << 8 | lsb);
            ++begin;
            m_outputSize++;
        }
        i++;
    }
}

}