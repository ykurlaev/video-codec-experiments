#ifndef CODEC_CODEC_H
#define CODEC_CODEC_H

#include <cstdio>
#include <iostream>
#include <vector>
#include "bytearrayserializer.h"
#include "constantvalueiterator.h"
#include "dct.h"
#include "findaverage.h"
#include "findsad.h"
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

class Codec
{
    public:
        static bool encode(FILE *input, FILE *output,
                           coord_t width, coord_t height,
                           uint8_t quality, Format::QuantizationMode mode,
                           bool silent, std::ostream *error = &std::cerr);
        static bool decode(FILE *input, FILE *output,
                           bool silent, std::ostream *error = &std::cerr);
    private:
        static const size_t SIZE = 16;
        enum Direction { ENCODE, DECODE };
        Codec(Direction direction, FILE *input, FILE *output,
              Format::HeaderParams params,
              bool silent, std::ostream *error);
        bool encodeInternal();
        void process(Format::MacroblockMode mode);
        size_t processI();
        size_t processP();
        size_t processB();
        bool decodeInternal();
        void recalculateQuality(size_t currentBitrate);
        Direction m_direction;
        FILE *m_input;
        FILE *m_output;
        bool m_silent;
        std::ostream *m_error;
        Format::HeaderParams m_params;
        Frame<SIZE, SIZE> m_current;
        Frame<SIZE, SIZE> m_previous;
        Frame<SIZE, SIZE> m_otherPrevious;
        Frame<SIZE, SIZE> m_next;
        std::vector<Frame<SIZE, SIZE>> m_BFrames;
        std::vector<uint8_t> m_uncompressed;
        std::vector<uint8_t> m_IPrecompressed;
        std::vector<uint8_t> m_PPrecompressed;
        std::vector<uint8_t> m_P2Precompressed;
        Format m_format;
        Context m_context;
        std::vector<Macroblock> m_macroblocks;
        FindSAD m_findSAD;
        ByteArraySerializer m_byteArraySerializer; //###
        size_t m_desiredBitrate;
};

}

#endif //CODEC_CODEC_H
