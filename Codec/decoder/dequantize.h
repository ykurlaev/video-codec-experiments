#ifndef CODEC_DEQUANTIZE_H
#define CODEC_DEQUANTIZE_H

#include "../util.h"

namespace Codec
{

class Dequantize
{
    public:
        Dequantize(bool flat, uint8_t param/*, Frame<>::data_t dcPred = 0*/);
        void setDcPred(Frame<>::data_t dcPred);
        template <typename Iterator>
        void operator()(Iterator begin, Iterator end);
    private:
        bool m_flat;
        uint8_t m_param;
        //Frame<>::data_t m_dcPred;
        bool m_np;
        int m_table[64];
};

}

#include "dequantize.inl"

#endif //CODEC_DEQUANTIZE_H