#ifndef CODEC_QUANTIZATION_H
#define CODEC_QUANTIZATION_H

#include "util.h"

namespace Codec
{

class Quantization
{
    public:
        Quantization(bool flat, uint8_t param);
        template <typename Iterator>
        void applyForward(Iterator begin, Iterator end);
        template <typename Iterator>
        void applyReverse(Iterator begin, Iterator end);
    private:
        bool m_flat;
        uint8_t m_param;
        int m_table[64];
};

}

#include "quantization.inl"

#endif //CODEC_QUANTIZATION_H
