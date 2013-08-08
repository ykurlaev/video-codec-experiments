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
        template <size_t M>
        struct ZigZagScanInternal;
        static ZigZagScanInternal<N> sm_internal;
};

template <size_t N>
template <size_t M>
struct ZigZagScan<N>::ZigZagScanInternal
{
    ZigZagScanInternal();
    typename Frame<M>::coord_t m_scan[M * M];
};

}

#include "zigzagscan.inl"

#endif //CODEC_ZIGZAGSCAN_H