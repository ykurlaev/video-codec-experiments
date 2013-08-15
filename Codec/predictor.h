#ifndef CODEC_PREDICTOR_H
#define CODEC_PREDICTOR_H

namespace Codec
{

class Predictor
{
    public:
        template <typename Iterator1, typename Iterator2>
        void applyForward(Iterator1 begin, Iterator1 end, Iterator2 neighbor);
        template <typename Iterator1, typename Iterator2>
        void applyReverse(Iterator1 begin, Iterator1 end, Iterator2 neighbor);
};

}

#include "predictor.inl"

#endif //CODEC_PREDICTOR_H