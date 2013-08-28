#ifndef CODEC_FINDSAD_H
#define CODEC_FINDSAD_H

#include "util.h"

namespace Codec
{

class FindSAD
{
    public:
        template <typename Iterator1, typename Iterator2>
        uint32_t operator()(Iterator1 begin, Iterator1 end, Iterator2 second);
};

}

#include "findsad.inl"

#endif //CODEC_FINDSAD_H
