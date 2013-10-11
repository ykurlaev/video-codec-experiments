#ifndef CODEC_QUANTIZATION_H
#define CODEC_QUANTIZATION_H

#include "format.h"
#include "util.h"

namespace Codec
{

template <size_t N>
class Quantization
{
    public:
        Quantization();
        void setParams(Format::QuantizationMode mode, uint8_t quality);
        template <typename Iterator>
        void applyForward(Iterator begin, Iterator end);
        template <typename Iterator>
        void applyReverse(Iterator begin, Iterator end);
    private:
        bool m_paramsSet;
        uint8_t m_quality;
        Format::QuantizationMode m_mode;
        int m_table[64 * N * N];
};

}

#include "quantization.inl"

#endif //CODEC_QUANTIZATION_H
