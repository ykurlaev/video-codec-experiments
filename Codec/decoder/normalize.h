#ifndef CODEC_NORMALIZE_H
#define CODEC_NORMALIZE_H

#include "../util.h"

namespace Codec
{

class Normalize
{
    public:
        template <typename Iterator>
        void operator()(Iterator begin, Iterator end);
};

}

#include "normalize.inl"

#endif //CODEC_NORMALIZE_H