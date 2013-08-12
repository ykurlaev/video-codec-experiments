#include <stdexcept>

namespace Codec
{

////###DEBUG
//long long max_dc = 0;
//long long max_ac = 0;
//long long min_dc = 1000000000000000000;
//long long min_ac = 1000000000000000000;
//long long sum_dc = 0;
//long long sum_ac = 0;
//long long dcc = 0;
//long long acc = 0;
////###

inline Quantize::Quantize(bool flat, uint8_t param/*, Frame<>::data_t dcPred*/)
    : m_flat(flat), m_param(param)/*, m_dcPred(dcPred)*/
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
        for(size_t i = 0; i < sizeof(m_table) / sizeof(*m_table); i++)
        {
            m_table[i] = ((n * JPEG_TABLE[i]) + 500) / 100;
        }
    }
}
//
//inline void Quantize::setDcPred(Frame<>::data_t dcPred)
//{
//    m_dcPred = dcPred;
//}

template <typename Iterator>
inline void Quantize::operator()(Iterator begin, Iterator end)
{
    size_t i = 0;
    for(; begin != end; ++begin)
    {
        //if(i == 0)
        //{
        //    //std::cerr << "\nQ: DC=" << *begin << ", pred=" << m_dcPred;
        //    Frame<>::data_t tmp = *begin;
        //    *begin -= m_dcPred;
        //    m_dcPred = tmp;
        //    //std::cerr << ", newDC=" << *begin << ", newpred=" << m_dcPred;
        //}
        ////###DEBUG
        //if(i != 0)
        //{
        //    if(*begin > max_ac)
        //    {
        //        max_ac = *begin;
        //    }
        //    if(*begin < min_ac)
        //    {
        //        min_ac = *begin;
        //    }
        //    sum_ac += *begin;
        //    acc += 1;
        //}
        //else
        //{
        //    if(*begin > max_dc)
        //    {
        //        max_dc = *begin;
        //    }
        //    if(*begin < min_dc)
        //    {
        //        min_dc = *begin;
        //    }
        //    sum_dc += *begin;
        //    dcc += 1;
        //}
        ////###
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

}