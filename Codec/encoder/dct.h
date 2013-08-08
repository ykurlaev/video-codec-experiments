#ifndef CODEC_DCT_H
#define CODEC_DCT_H

#include "../util.h"

namespace Codec
{

class DCT
{
    public:
        template <typename Iterator>
	    void operator()(Iterator begin, Iterator end);
};

}

#include "dct.inl"

#endif //CODEC_DCT_H