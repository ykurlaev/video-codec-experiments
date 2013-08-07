#ifndef CODEC_MOTIONESTIMATOR_H
#define CODEC_MOTIONESTIMATOR_H

#include "../codec.h"
#include "../frame/frame.h"

namespace Codec
{

class MotionEstimator
{
    public:
        MotionEstimator();
        virtual Frame estimate(const Frame &current, const Frame &previous);
        virtual ~MotionEstimator();
    private:
        void estimateBlock(const Frame &block, const Frame &previous,
                           Frame::coord_t x, Frame::coord_t y,
                           Frame::data_t &xMotion, Frame::data_t &yMotion) const;
};

}

#endif //CODEC_MOTIONESTIMATOR_H