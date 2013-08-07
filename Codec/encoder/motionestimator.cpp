#include "motionestimator.h"
#include "../encoder/splittoblocks.h"

using namespace Codec;

using std::vector;

MotionEstimator::MotionEstimator()
{}

Frame MotionEstimator::estimate(const Frame &current, const Frame &previous)
{
    SplitToBlocks splitToBlocks(16);
    Frame::coord_t xCount = (current.getWidth() / 16 + 15) * 16;
    Frame::coord_t yCount = (current.getHeight() / 16 + 15) * 16;
    Frame result(xCount, yCount, 2);
    vector<Frame> blocks = current >> splitToBlocks;
    for(Frame::coord_t x = 0; x < xCount; x++)
    {
        for(Frame::coord_t y = 0; y < yCount; y++)
        {
            estimateBlock(blocks[x * yCount + y], previous, x * 16, y * 16, result(x, y, 0), result(x, y, 1));
        }
    }
    return result;
}

void MotionEstimator::estimateBlock(const Frame &block, const Frame &previous,
                                    Frame::coord_t x, Frame::coord_t y,
                                    Frame::data_t &xMotion, Frame::data_t &yMotion) const 
{

}

MotionEstimator::~MotionEstimator()
{}