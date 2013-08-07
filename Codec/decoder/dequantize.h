#ifndef CODEC_DEQUANTIZE_H
#define CODEC_DEQUANTIZE_H

#include "../region/region.h"
#include "../util.h"

namespace Codec
{

class Dequantize
{
    public:
        Dequantize(bool flat, uint8_t param);
        virtual void operator()(Region &region);
        virtual ~Dequantize();
    private:
        bool m_flat;
        uint8_t m_param;
};

}

#endif //CODEC_DEQUANTIZE_H