#ifndef CODEC_DEQUANTIZE_H
#define CODEC_DEQUANTIZE_H

#include "../util.h"

namespace Codec
{

class Dequantize
{
    public:
        Dequantize(bool flat, uint8_t param);
        template <typename Iterator>
        void operator()(Iterator begin, Iterator end);
    private:
        bool m_flat;
        uint8_t m_param;
};

}

#include "dequantize.inl"

#endif //CODEC_DEQUANTIZE_H