#ifndef CODEC_QUANTIZE_H
#define CODEC_QUANTIZE_H

//#include "../frame.h"
#include "../util.h"

namespace Codec
{

class Quantize
{
    public:
        Quantize(bool flat, uint8_t param/*, Frame<>::data_t dcPred = 0*/);
        void setDcPred(Frame<>::data_t dcPred);
        template <typename Iterator>
        void operator()(Iterator begin, Iterator end);
    private:
        bool m_flat;
        uint8_t m_param;
        //Frame<>::data_t m_dcPred;
        int m_table[64];
};

}

#include "quantize.inl"

#endif //CODEC_QUANTIZE_H