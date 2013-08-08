#ifndef CODEC_UNDCT_H
#define CODEC_UNDCT_H

#include "../util.h"

namespace Codec
{

class UnDCT
{
    public:
        template <typename Iterator>
        void operator()(Iterator begin, Iterator end);
};

}

#include "undct.inl"

#endif //CODEC_UNDCT_H