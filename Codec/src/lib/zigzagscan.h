#ifndef CODEC_ZIGZAGSCAN_H
#define CODEC_ZIGZAGSCAN_H

#include "util.h"

namespace Codec
{

template <size_t MIN_N, size_t MAX_N = MIN_N>
class ZigZagScan
{
    public:
        static const coord_t *getScan();
    private:
        ZigZagScan();
        ZigZagScan(const ZigZagScan &);
        void operator=(const ZigZagScan &);
        coord_t m_scan[MAX_N * MAX_N];
};

}

#include "zigzagscan.inl"

#endif //CODEC_ZIGZAGSCAN_H
