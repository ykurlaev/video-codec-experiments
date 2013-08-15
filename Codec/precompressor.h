#ifndef CODEC_PRECOMPRESSOR_H
#define CODEC_PRECOMPRESSOR_H

#include "../util.h"

namespace Codec
{

class Precompressor
{
    public:
        static const size_t MAX_BYTES = 2;
        Precompressor(uint8_t *byteArray = NULL);
        void setByteArray(uint8_t *byteArray);
        template <typename Iterator>
        size_t applyForward(Iterator begin, Iterator end);
        template <typename Iterator>
        size_t applyReverse(Iterator begin, Iterator end);
    private:
        uint8_t *m_byteArray;
};

}

#include "precompressor.inl"

#endif //CODEC_PRECOMPRESSOR_H