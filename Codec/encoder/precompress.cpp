#include "precompress.h"
#include <exception>

namespace Codec
{

using std::exception;

Precompress::Precompress()
{}

size_t Precompress::operator()(const char *from, char *to, size_t size)
{
    if(size % 2 != 0)
    {
        throw exception("Size must be dividable by 2");
    }
    const uint8_t *fromu = reinterpret_cast<const uint8_t *>(from);
    uint8_t *tou = reinterpret_cast<uint8_t *>(to);
    size_t result = 0;
    uint8_t zeros = 0;
    for(size_t i = 0; i < size; i += 2)
    {
        if(fromu[i] == 0 && fromu[i + 1] == 0 && zeros != 0x3F)
        {
            zeros++;
        }
        else
        {
            if(zeros != 0)
            {
                tou[result] = 0xC0 | zeros;
                result++;
                zeros = 0;
            }
            bool sign = fromu[i + 1] >> 7 == 1;
            if(from[i + 1] == (sign ? ~0 : 0)
               && from[i] >> 6 == (sign ? 0x3 : 0))
            {
                tou[result] = fromu[i] & 0x7F;
                result++;
            }
            else
            {
                tou[result] = (fromu[i + 1] & 0x3F) | 0x80;
                tou[result + 1] = fromu[i];
                result += 2;
            }
        }
    }
    return result;
}

Precompress::~Precompress()
{}

}