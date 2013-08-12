#ifndef CODEC_FINDRESIDUAL_H
#define CODEC_FINDRESIDUAL_H

namespace Codec
{

class FindResidual
{
    public:
        template <typename Iterator1, typename Iterator2>
        void operator()(Iterator1 begin, Iterator1 end, Iterator2 neighbor);
};

}

#include "findresidual.inl"

#endif //CODEC_FINDRESIDUAL_H