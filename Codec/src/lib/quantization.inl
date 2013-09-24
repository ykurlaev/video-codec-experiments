#include <stdexcept>

namespace Codec
{

template <size_t N>
inline Quantization<N>::Quantization(bool flat, uint8_t param)
    : m_flat(flat), m_param(param)
{
    if(m_param < 1 || (!m_flat && m_param > 100))
    {
       throw std::runtime_error("Invalid quality value");
    }
    //standatd JPEG quantization matrix
    static const uint8_t JPEG_TABLE[] = { 16, 11, 10, 16, 24, 40, 51, 61,
                                          12, 12, 14, 19, 26, 58, 60, 55,
                                          14, 13, 16, 24, 40, 57, 69, 56,
                                          14, 17, 22, 29, 51, 87, 80, 62,
                                          18, 22, 37, 56, 68, 109, 103, 77,
                                          24, 35, 55, 64, 81, 104, 113, 92,
                                          49, 64, 78, 87, 103, 121, 120, 101,
                                          72, 92, 95, 98, 112, 100, 103, 99 };
    if(!m_flat)
    {
        int n = (m_param < 50) ? (5000 / m_param) : (200 - 2 * m_param);
        for(size_t y = 0; y < N; y++)
        {
            for(size_t x = 0; x < N; x++)
            {
                for(size_t i = 0; i < 64; i++)
                {
                    m_table[8 * N * (8 * y + i / 8) + 8 * x + i % 8] =
                        ((n * JPEG_TABLE[i]) + 500) / 100;
                }
            }
        }
    }
}

template <size_t N>
template <typename Iterator>
inline void Quantization<N>::applyForward(Iterator begin, Iterator end)
{
    size_t i = 0;
    for(; begin != end; ++begin)
    {
        if(m_flat)
        {
            *begin /= m_param;
        }
        else
        {
            *begin /= m_table[i];
        }
        i = (i + 1) % (sizeof(m_table) / sizeof(*m_table));
    }
}

template <size_t N>
template <typename Iterator>
inline void Quantization<N>::applyReverse(Iterator begin, Iterator end)
{
    size_t i = 0;
    for(; begin != end; ++begin)
    {
        if(m_flat)
        {
            *begin *= m_param;
        }
        else
        {
            *begin *= m_table[i];
        }
        i = (i + 1) % (sizeof(m_table) / sizeof(*m_table));
    }
}

}
