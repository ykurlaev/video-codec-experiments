#include "unprecompress.h"
#include <cstring>

namespace Codec
{

UnPrecompress::UnPrecompress()
{}

void UnPrecompress::operator()(const char *from, char *to, size_t size)
{
    const uint8_t *fromu = reinterpret_cast<const uint8_t *>(from);
    uint8_t *tou = reinterpret_cast<uint8_t *>(to);
    size_t out = 0;
    for(size_t i = 0; i < size; i++)
    {
        if(fromu[i] >> 6 == 0x3)
        {
            uint8_t zeros = fromu[i] & 0x3F;
            memset(&tou[out], 0, zeros * 2);
            out += zeros * 2;
        }
        else
        {
            if(fromu[i] >> 7 == 0)
            {
                uint8_t sign = (fromu[i] >> 6 == 1) ? ~0 : 0;
                tou[out + 1] = sign;
                tou[out] = (fromu[i] & 0x7F) | (sign & 0x80);
            }
            else
            {
                tou[out + 1] = (fromu[i] & 0x3F) | (((fromu[i] & 0x20) == 0) ? 0 : 0xC0);
                tou[out] = fromu[i + 1];
                i++;
            }
            out += 2;
        }
    }
}

UnPrecompress::~UnPrecompress()
{}

}