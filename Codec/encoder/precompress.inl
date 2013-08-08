namespace Codec
{

inline Precompress::Precompress(uint8_t *output)
    : m_output(output), m_outputSize(0)
{}

inline void Precompress::setOutput(uint8_t *output)
{
    m_output = output;
}

inline size_t Precompress::getOutputSize() const
{
    return m_outputSize;
}

template <typename Iterator>
inline void Precompress::operator()(Iterator begin, Iterator end)
{
    if(m_output == NULL)
    {
        return;
    }
    m_outputSize = 0;
    uint8_t zeros = 0;
    for(; begin != end; ++begin)
    {
        typename IntN<sizeof(typename Iterator::value_type)>::u u = *begin;
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
                m_output[m_outputSize] = 0xC0 | zeros;
                m_outputSize++;
                zeros = 0;
            }
            bool sign = msb >> 7 == 1;
            if(msb == (sign ? ~0 : 0) && lsb >> 6 == (sign ? 0x3 : 0))
            {
                m_output[m_outputSize] = lsb & 0x7F;
                m_outputSize++;
            }
            else
            {
                m_output[m_outputSize] = (msb & 0x3F) | 0x80;
                m_output[m_outputSize + 1] = lsb;
                m_outputSize += 2;
            }
        }
    }
}

}