#ifndef CODEC_UNPRECOMPRESS_H
#define CODEC_UNPRECOMPRESS_H

#include "../util.h"

namespace Codec
{

class UnPrecompress
{
    public:
        static const size_t MAX_BYTES = 2;
        UnPrecompress(uint8_t *input = NULL);
        void setInput(uint8_t *input);
        size_t getOutputSize() const;
        template <typename Iterator>
        void operator()(Iterator begin, Iterator end);
    private:
        uint8_t *m_input;
        size_t m_outputSize;
};

}

#include "unprecompress.inl"

#endif //CODEC_PRECOMPRESS_H