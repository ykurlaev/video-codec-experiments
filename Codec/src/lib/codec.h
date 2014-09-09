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
#include "motionestimator.h"
#include "normalize.h"
#include "precompressor.h"
#include "predictor.h"
#include "quantization.h"
#include "util.h"
#include "zigzagscan.h"
#include "zlibcompress.h"
#include "zlibdecompress.h"

namespace Codec
{

class Codec
{
    public:
        static Codec initEncode(FILE *input, FILE *output,
                                Frame<>::coord_t width, Frame<>::coord_t height,
                                uint8_t quality, bool flat,
                                bool silent, std::ostream *error = &std::cerr);
        static Codec initDecode(FILE *input, FILE *output,
                                bool silent, std::ostream *error = &std::cerr);
        bool operator()();
    private:
        enum Direction { ENCODE, DECODE };
        Codec(Direction direction, FILE *input, FILE *output,
              Frame<>::coord_t width, Frame<>::coord_t height,
              uint8_t quality, bool flat, bool silent, std::ostream *error);
        bool encode();
        bool decode();
        void processMacroblockForward(Frame<>::coord_t macroblock, bool forceI);
        void processMacroblockReverse(Frame<>::coord_t macroblock);
        Direction m_direction;
        FILE *m_input;
        FILE *m_output;
        Frame<>::coord_t m_width;
        Frame<>::coord_t m_height;
        uint8_t m_quality;
        bool m_flat;
        bool m_silent;
        std::ostream *m_error;
        const Frame<>::coord_t *m_zigZagScan;
        ByteArraySerializer m_byteArraySerializer;
        FindAverage m_findAverage;
        FindSAD m_findSAD;
        DCT m_dct;
        MotionEstimator m_motionEstimator;
        Normalize m_normalize;
        Predictor m_predictor;
        Precompressor m_precompressor;
        Quantization<2> m_quantization;
        ZlibCompress m_zlibCompress;
        ZlibDecompress m_zlibDecompress;
        Frame<16> m_current;
        Frame<16> m_previous;
        std::vector<uint8_t> m_uncompressed;
        std::vector<uint8_t> m_precompressed;
        std::vector<uint8_t> m_compressed;
        std::vector<uint8_t> m_macroblockIsInter;
        std::vector<int8_t> m_motionVectorsX;
        std::vector<int8_t> m_motionVectorsY;
        std::vector<uint8_t> m_precompressedMeta;
        std::vector<uint8_t> m_compressedMeta;
        Frame<>::data_t m_prevMacroblockAverage;
        Frame<>::coord_t m_macroblockWidth;
};

}

#endif //CODEC_CODEC_H