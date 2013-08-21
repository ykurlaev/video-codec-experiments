#ifndef CODEC_ZIGZAGSCAN_H
#define CODEC_ZIGZAGSCAN_H

#include "frame.h"
#include "util.h"

namespace Codec
{

template <size_t N>
class ZigZagScan
{
    public:
        static const typename Frame<N>::coord_t *getScan();
    private:
        ZigZagScan();
        ZigZagScan(const ZigZagScan &);
        void operator=(const ZigZagScan &);
        typename Frame<N>::coord_t m_scan[N * N];
};

}

#include "zigzagscan.inl"

#endif //CODEC_ZIGZAGSCAN_H
