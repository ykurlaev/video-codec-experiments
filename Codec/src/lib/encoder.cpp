#include "encoder.h"
#include <cstdio>
#include <cstring>
#ifdef MEASURE_TIME
#include <ctime>
#endif
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <exception>
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
#include "zigzagscan.h"
#include "zlibcompress.h"

namespace Codec
{

using std::fopen;
using std::strncmp;
#ifdef MEASURE_TIME
using std::clock;
#endif
using std::vector;
using std::cerr;
using std::istringstream;
using std::copy;
using std::swap;
using std::exception;

int encode(int argc, char *argv[])
{
    try
    {
        if(argc < 4)
        {
            return 1;
        }
        FILE *in = fopen(argv[0], "rb");
        if(!in)
        {
            cerr << "Error: Can't open input file " << argv[0] << "\n";
            return 2;
        }
        FILE *out = fopen(argv[3], "wb");
        if(!out)
        {
            cerr << "Error: Can't open output file " << argv[3] << "\n";
            return 2;
        }
        Frame<8>::coord_t width;
        Frame<8>::coord_t height;
        istringstream(argv[1]) >> width;
        istringstream(argv[2]) >> height;
        uint8_t quality = 50;
        if(argc > 4)
        {
            uint32_t uquality = 0;
            istringstream(argv[4]) >> uquality;
            if(uquality != 0)
            {
                quality = static_cast<uint8_t>(uquality);
            }
        }
        bool flat = false;
        if(argc > 5)
        {
            uint32_t uflat = 0;
            istringstream(argv[5]) >> uflat;
            flat = uflat != 0;
        }
        bool silent = false;
        if(argc > 6 && !strncmp(argv[6], "-q", 3))
        {
            silent = true;
        }
        ByteArraySerializer byteArraySerializer;
        byteArraySerializer.serializeUint32(width, out);
        byteArraySerializer.serializeUint32(height, out);
        byteArraySerializer.serializeUint32(quality, out);
        byteArraySerializer.serializeUint32(flat ? 1 : 0, out);
        Frame<8, 16> current(width, height);
        Frame<8, 16> previous(width, height);
        vector<uint8_t> uncompressed(current.getWidth() * current.getHeight());
        vector<uint8_t> precompressed(current.getAlignedWidth() * current.getAlignedHeight() * Precompressor::MAX_BYTES);
        vector<uint8_t> compressed(precompressed.size());
        vector<uint8_t> macroblockIsInter((current.getAlignedWidth() * current.getAlignedHeight()) / (16 * 16) / 8);
        vector<int8_t> motionVectorsX((current.getAlignedWidth() * current.getAlignedHeight()) / (16 * 16));
        vector<int8_t> motionVectorsY((current.getAlignedWidth() * current.getAlignedHeight()) / (16 * 16));
        vector<uint8_t> precompressedMeta((macroblockIsInter.size() + motionVectorsX.size()
                                           + motionVectorsY.size()) * Precompressor::MAX_BYTES);
        vector<uint8_t> compressedMeta(precompressedMeta.size());
        FindAverage findAverage;
        MotionEstimator motionEstimator;
        Predictor predictor;
        DCT dct;
        Quantization quantization(flat, quality);
        const Frame<>::coord_t *zigZagScan = ZigZagScan<8>::getScan();
        Precompressor precompressor;
        ZlibCompress zlibCompress;
        Normalize normalize;
        if(!silent)
        {
            cerr << "Compressing... ";
        }
#ifdef MEASURE_TIME
        clock_t begin = clock();
#endif
        for(unsigned count = 1; ; count++)
        {
            swap(previous, current);
            current.clear();
            if(byteArraySerializer.deserializeByteArray(in, &uncompressed[0], uncompressed.size(), false)
               != uncompressed.size())
            {
                break;
            }
            if(!silent)
            {
                cerr << count << " ";
            }
            current.fromByteArray(&uncompressed[0]);
            precompressor.setByteArray(&precompressed[0]);
            for(Frame<>::coord_t block = 0; block < (current.getAlignedWidth() * current.getAlignedHeight())
                                                    / (8 * 8); block += 4)
            {
                uint32_t sad;
                Frame<>::coord_t macroblockWidth = current.getAlignedWidth() / 16;
                Frame<>::coord_t currentX = ((block / 4) % macroblockWidth) * 16,
                                 currentY = ((block / 4) / macroblockWidth) * 16;
                motionEstimator(current, previous, block, &motionVectorsX[block / 4],
                                &motionVectorsY[block / 4], &sad);

                if(sad < 10 * 16 * 16)
                {
                    macroblockIsInter[(block / 4) / 8] |= (1 << ((block / 4) % 8));
                    predictor.applyForward(current.regionBegin(currentX, currentY, 16, 16),
                                           current.regionEnd(),
                                           previous.regionBegin(currentX + motionVectorsX[block / 4],
                                                                currentY + motionVectorsY[block / 4],
                                                                16, 16));
                }
                else
                {
                    macroblockIsInter[(block / 4) / 8] &= ~(1 << ((block / 4) % 8));
                    motionVectorsX[block / 4] = 0;
                    motionVectorsY[block / 4] = 0;
                }
                if(block != 0 && (((macroblockIsInter[(block / 4) / 8] & (1 << ((block / 4) % 8))) != 0) ==
                                  ((macroblockIsInter[((block - 4) / 4) / 8] & (1 << (((block - 4) / 4) % 8))) != 0)))
                {
                    predictor.applyForward(current.horizontalBegin(block), current.horizontalBegin(block + 4),
                                           makeConstantValueIterator(findAverage(current.horizontalBegin(block - 4),
                                                                                 current.horizontalBegin(block))));
                }
                else
                {
                    if((macroblockIsInter[(block / 4) / 8] & (1 << ((block / 4) % 8))) == 0)
                    {
                        predictor.applyForward(current.horizontalBegin(block), current.horizontalBegin(block + 4),
                                               makeConstantValueIterator(128));
                    }
                }
                dct.applyForward(current.horizontalBegin(block), current.horizontalBegin(block + 4));
                dct.applyForward(current.verticalBegin(block), current.verticalBegin(block + 4));
                quantization.applyForward(current.horizontalBegin(block), current.horizontalBegin(block + 4));
                precompressor.applyForward(current.scanningBegin(zigZagScan, block),
                                           current.scanningBegin(zigZagScan, block + 4));
                //###
                quantization.applyReverse(current.horizontalBegin(block), current.horizontalBegin(block + 4));
                dct.applyReverse(current.horizontalBegin(block), current.horizontalBegin(block + 4));
                dct.applyReverse(current.verticalBegin(block), current.verticalBegin(block + 4));
                if(block != 0 && (((macroblockIsInter[(block / 4) / 8] & (1 << ((block / 4) % 8))) != 0) ==
                                  ((macroblockIsInter[((block - 4) / 4) / 8] & (1 << (((block - 4) / 4) % 8))) != 0)))
                {
                    predictor.applyReverse(current.horizontalBegin(block), current.horizontalBegin(block + 4),
                                           makeConstantValueIterator(findAverage(current.horizontalBegin(block - 4),
                                                                                 current.horizontalBegin(block))));
                }
                else
                {
                    if((macroblockIsInter[(block / 4) / 8] & (1 << ((block / 4) % 8))) == 0)
                    {
                        predictor.applyReverse(current.horizontalBegin(block), current.horizontalBegin(block + 4),
                                               makeConstantValueIterator(128));
                    }
                }
                if((macroblockIsInter[(block / 4) / 8] & (1 << ((block / 4) % 8))) != 0)
                {
                    predictor.applyReverse(current.regionBegin(currentX, currentY, 16, 16),
                                           current.regionEnd(),
                                           previous.regionBegin(currentX + motionVectorsX[block / 4],
                                                                currentY + motionVectorsY[block / 4],
                                                                16, 16));
                }
                //###
            }
            normalize(current.horizontalBegin(), current.horizontalEnd());
            uint32_t precompressedSize = precompressor.getBytesProcessed();
            precompressor.setByteArray(&precompressedMeta[0]);
            precompressor.applyForward(&macroblockIsInter[0], &macroblockIsInter[0] + macroblockIsInter.size());
            precompressor.applyForward(&motionVectorsX[0], &motionVectorsX[0] + motionVectorsX.size());
            precompressor.applyForward(&motionVectorsY[0], &motionVectorsY[0] + motionVectorsY.size());
            uint32_t compressedMetaSize = zlibCompress(&precompressedMeta[0], &compressedMeta[0],
                                                       precompressor.getBytesProcessed(), compressedMeta.size());
            byteArraySerializer.serializeByteArray(&compressedMeta[0], compressedMetaSize, out);
            uint32_t compressedSize = zlibCompress(&precompressed[0], &compressed[0],
                                                   precompressedSize, compressed.size());
            byteArraySerializer.serializeByteArray(&compressed[0], compressedSize, out);
        }
        if(!silent)
        {
            cerr << "Ok\n";
        }
#ifdef MEASURE_TIME
        clock_t end = clock();
        cerr << static_cast<double>(end - begin) / CLOCKS_PER_SEC << "\n";
#endif
    }
    catch(const exception &e)
    {
        cerr << "\nError: " << e.what() << "\n";
        return 2;
    }
    return 0;
}

}
