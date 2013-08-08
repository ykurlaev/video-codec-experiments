#ifndef CODEC_PRECOMPRESS_H
#define CODEC_PRECOMPRESS_H

#include "../util.h"

namespace Codec
{

class Precompress
{
    public:
        static const size_t MAX_BYTES = 2;
        Precompress(uint8_t *output = NULL);
        void setOutput(uint8_t *output);
        size_t getOutputSize() const;
        template <typename Iterator>
        void operator()(Iterator begin, Iterator end);
    private:
        uint8_t *m_output;
        size_t m_outputSize;
};

}

#include "precompress.inl"

#endif //CODEC_PRECOMPRESS_H