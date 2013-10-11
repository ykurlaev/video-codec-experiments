#ifndef CODEC_MACROBLOCK_H
#define CODEC_MACROBLOCK_H

#include "constantvalueiterator.h"
#include "dct.h"
#include "findaverage.h"
#include "findsad.h"
#include "format.h"
#include "frame.h"
#include "macroblock.h"
#include "motionestimator.h"
#include "normalize.h"
#include "precompressor.h"
#include "predictor.h"
#include "quantization.h"
#include "util.h"
#include "zigzagscan.h"

namespace Codec
{

using std::vector;

struct Context
{
    Context();
    static const coord_t SIZE = 16;
    const Frame<>::coord_t *m_scan;
    Predictor m_predictor;
    MotionEstimator m_motionEstimator;
    DCT m_dct;
    Quantization<SIZE / 8> m_quantization;
    Normalize m_normalize;
    FindAverage m_findAverage;
};

class Macroblock
{
    private:
        static const coord_t SIZE = 16;
    public:
        Macroblock(Frame<SIZE> *frame, Frame<SIZE> *previousFrame,
                   coord_t number, Macroblock **m_neighbors,
                   Codec::Context *context);
        void processForward(Format::MacroblockMode mode);
        size_t precompressTo(Format *format, uint8_t *buffer);
        void readFrom(Format *format);
        void processReverse();
        void chooseMode(Format::MacroblockMode mode);
    private:
        Frame<SIZE> *m_frame;
        Frame<SIZE> *m_previousFrame;
        coord_t m_number;
        coord_t m_x;
        coord_t m_y;
        Macroblock *m_neighbors[4];
        data_t m_PAverage;
        data_t m_IAverage;
        data_t m_average;
        Format::MacroblockParams m_params;
        Context *m_context;
};

}

#endif //CODEC_MACROBLOCK_H