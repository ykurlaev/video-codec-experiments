#ifndef CODEC_QUANTIZE_H
#define CODEC_QUANTIZE_H

#include "../region/region.h"
#include "../util.h"

namespace Codec
{

class Quantize
{
    public:
        Quantize(bool flat, uint8_t param);
        virtual void operator()(Region &region);
        virtual ~Quantize();
    private:
        bool m_flat;
        uint8_t m_param;
};

}

#endif //CODEC_QUANTIZE_H