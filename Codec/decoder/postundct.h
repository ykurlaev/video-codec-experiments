#ifndef CODEC_POSTUNDCT_H
#define CODEC_POSTUNDCT_H

#include "../util.h"

namespace Codec
{

class PostUnDCT
{
    public:
        template <typename Iterator>
        void operator()(Iterator begin, Iterator end);
};

}

#include "postundct.inl"

#endif //CODEC_POSTUNDCT_H