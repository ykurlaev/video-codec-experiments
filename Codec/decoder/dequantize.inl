#include <cassert>
#include <stdexcept>

namespace Codec
{

inline Dequantize::Dequantize(bool flat, uint8_t param)
    : m_flat(flat), m_param(param)
{}

template <typename Iterator>
inline void Dequantize::operator()(Iterator begin, Iterator end)
{
    //standatd JPEG quantization matrix
    static const uint8_t TABLE[] = { 16, 11, 10, 16, 24, 40, 51, 61,
                                     12, 12, 14, 19, 26, 58, 60, 55,
                                     14, 13, 16, 24, 40, 57, 69, 56,
                                     14, 17, 22, 29, 51, 87, 80, 62,
                                     18, 22, 37, 56, 68, 109, 103, 77,
                                     24, 35, 55, 64, 81, 104, 113, 92,
                                     49, 64, 78, 87, 103, 121, 120, 101,
                                     72, 92, 95, 98, 112, 100, 103, 99};
    if(!m_flat)
    {
        assert(sizeof(TABLE) / sizeof(*TABLE) == Iterator::BLOCK_SIZE * Iterator::BLOCK_SIZE);
        if(m_param < 1 || m_param > 100)
        {
            throw std::runtime_error("Invalid quality value");
        }
    }
    size_t i = 0;
    typename Iterator::value_type n = (m_param < 50) ? (5000 / m_param) : (200 - 2 * m_param);
    for(; begin != end; ++begin)
    {
        if(m_flat)
        {
            *begin *= m_param;
        }
        else
        {
            *begin *= ((n * TABLE[i] + 500) / 100);
        }
        if(i == 0)
        {
            *begin += (1 << Iterator::BLOCK_SIZE) * Iterator::BLOCK_SIZE * Iterator::BLOCK_SIZE / 2;
        }
        i = (i + 1) % (Iterator::BLOCK_SIZE * Iterator::BLOCK_SIZE);
    }
}

}