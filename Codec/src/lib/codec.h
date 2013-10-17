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
        static Codec initEncode(FILE *input, FILE *output,
                                coord_t width, coord_t height,
                                uint8_t quality, Format::QuantizationMode mode,
                                bool silent, std::ostream *error = &std::cerr);
        static Codec initDecode(FILE *input, FILE *output,
                                bool silent, std::ostream *error = &std::cerr);
        bool operator()();
    private:
        static const size_t SIZE = 16;
        enum Direction { ENCODE, DECODE };
        Codec(Direction direction, FILE *input, FILE *output,
              Format::HeaderParams params,
              bool silent, std::ostream *error);
        bool encode();
        bool decode();
        Direction m_direction;
        FILE *m_input;
        FILE *m_output;
        bool m_silent;
        std::ostream *m_error;
        Format::HeaderParams m_params;
        Frame<SIZE, SIZE> m_current;
        Frame<SIZE, SIZE> m_previous;
        std::vector<uint8_t> m_uncompressed;
        Format m_format;
        Context m_context;
        std::vector<Macroblock> m_macroblocks;
        FindSAD m_findSAD;
        ByteArraySerializer m_byteArraySerializer; //###
};

}

#endif //CODEC_CODEC_H
