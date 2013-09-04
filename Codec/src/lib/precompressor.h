#ifndef CODEC_PRECOMPRESSOR_H
#define CODEC_PRECOMPRESSOR_H

#include <iterator>
#include "util.h"

namespace Codec
{

class Precompressor
{
    public:
        static const size_t MAX_BYTES = 2;
        Precompressor();
        void setByteArray(uint8_t *byteArray);
        size_t getBytesProcessed() const;
        template <typename Iterator>
        void applyForward(Iterator begin, Iterator end);
        template <typename Iterator>
        void applyReverse(Iterator begin, Iterator end);
    private:
        uint8_t *m_byteArray;
        size_t m_bytesProcessed;
        template <typename Iterator>
        void applyForward16Signed(Iterator begin, Iterator end);
        template <typename Iterator>
        void applyReverse16Signed(Iterator begin, Iterator end);
        template <typename Iterator>
        void applyForward8Unsigned(Iterator begin, Iterator end);
        template <typename Iterator>
        void applyReverse8Unsigned(Iterator begin, Iterator end);

};

}

#include "precompressor.inl"

#endif //CODEC_PRECOMPRESSOR_H
