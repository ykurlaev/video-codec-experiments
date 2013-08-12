#ifndef CODEC_ADDRESIDUAL_H
#define CODEC_ADDRESIDUAL_H

namespace Codec
{

class AddResidual
{
    public:
        template <typename Iterator1, typename Iterator2>
        void operator()(Iterator1 begin, Iterator1 end, Iterator2 neighbor);
};

}

#include "addresidual.inl"

#endif //CODEC_ADDRESIDUAL_H