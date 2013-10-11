#ifndef CODEC_MOTIONESTIMATOR_H
#define CODEC_MOTIONESTIMATOR_H

#include "frame.h"
#include "util.h"

namespace Codec
{

class MotionEstimator
{
    private:
        typedef Frame<>::coord_t coord_t;
        typedef MakeSigned<coord_t>::t scoord_t;
    public:
        template <uint32_t MIN_N, uint32_t MAX_N>
        void operator()(Frame<MIN_N, MAX_N> &current, Frame<MIN_N, MAX_N> &previous, coord_t macroblock,
                        int8_t *x, int8_t *y, uint32_t *sad = NULL);
};

}

#include "motionestimator.inl"

#endif //CODEC_MOTIONESTIMATOR_H
