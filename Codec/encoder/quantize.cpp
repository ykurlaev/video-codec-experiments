#include "quantize.h"
#include <exception>
//###DEBUG
#include <iostream>
//###

namespace Codec
{

//###DEBUG
long long max_dc = 0;
long long max_ac = 0;
long long min_dc = 1000000000000000000;
long long min_ac = 1000000000000000000;
long long sum_dc = 0;
long long sum_ac = 0;
long long dcc = 0;
long long acc = 0;
//###

using std::exception;

Quantize::Quantize(bool flat, uint8_t param)
    : m_flat(flat), m_param(param)
{}

void Quantize::operator()(Region &region)
{
    if(region.getWidth() != 8 || region.getHeight() != 8)
    {
        throw exception("Region must be 8x8");
    }
    //standatd JPEG quantization matrix
    static const uint8_t TABLE[] = { 16, 11, 10, 16, 24, 40, 51, 61,
                                     12, 12, 14, 19, 26, 58, 60, 55,
                                     14, 13, 16, 24, 40, 57, 69, 56,
                                     14, 17, 22, 29, 51, 87, 80, 62,
                                     18, 22, 37, 56, 68, 109, 103, 77,
                                     24, 35, 55, 64, 81, 104, 113, 92,
                                     49, 64, 78, 87, 103, 121, 120, 101,
                                     72, 92, 95, 98, 112, 100, 103, 99};
    size_t i = 0;
    Region::data_t n = (m_param < 50) ? (5000 / m_param) : (200 - 2 * m_param);
    for(Region::iterator it = region.begin(); it != region.end(); ++it)
    {
        if(i == 0)
        {
            *it -= 8096;
        }
        ////###DEBUG
        //if(i != 0)
        //{
        //    if(*it > max_ac)
        //    {
        //        max_ac = *it;
        //    }
        //    if(*it < min_ac)
        //    {
        //        min_ac = *it;
        //    }
        //    sum_ac += *it;
        //    acc += 1;
        //}
        //else
        //{
        //    if(*it > max_dc)
        //    {
        //        max_dc = *it;
        //    }
        //    if(*it < min_dc)
        //    {
        //        min_dc = *it;
        //    }
        //    sum_dc += *it;
        //    dcc += 1;
        //}
        ////###
        if(m_flat)
        {
            *it /= m_param;
        }
        else
        {
            *it /= ((n * TABLE[i] + 500) / 100);
        }
        i++;
    }
}

Quantize::~Quantize()
{}

}