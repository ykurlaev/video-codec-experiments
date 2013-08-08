#ifndef CODEC_QUANTIZE_H
#define CODEC_QUANTIZE_H

#include "../util.h"

namespace Codec
{

class Quantize
{
    public:
        Quantize(bool flat, uint8_t param);
        template <typename Iterator>
        void operator()(Iterator begin, Iterator end);
    private:
        bool m_flat;
        uint8_t m_param;
};

}

#include "quantize.inl"

#endif //CODEC_QUANTIZE_H