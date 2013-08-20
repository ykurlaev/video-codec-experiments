#include "decoder.h"
#include <cstdio>
#include <cstring>
#ifdef MEASURE_TIME
#include <ctime>
#endif
#include <vector>
#include <iostream>
#include <algorithm>
#include <exception>
#include "bytearrayserializer.h"
#include "dct.h"
#include "frame.h"
#include "normalize.h"
#include "precompressor.h"
#include "predictor.h"
#include "quantization.h"
#include "zigzagscan.h"
#include "zlibdecompress.h"

namespace Codec
{

using std::cerr;
using std::vector;
using std::copy;
using std::exception;

int decode(int argc, char *argv[])
{
    try
    {
        if(argc < 2)
        {
            return 1;
        }
        FILE *in = fopen(argv[0], "rb");
        if(!in)
        {
            cerr << "Error: Can't open input file " << argv[0] << "\n";
            return 2;
        }
        FILE *out = fopen(argv[1], "wb");
        if(!out)
        {
            cerr << "Error: Can't open output file " << argv[1] << "\n";
            return 2;
        }
        bool silent = false;
        if(argc > 2 && !strncmp(argv[2], "-q", 3))
        {
            silent = true;
        }
        uint32_t uwidth;
        uint32_t uheight;
        uint32_t uquality;
        uint32_t uflat;
        ByteArraySerializer byteArraySerializer;
        byteArraySerializer.deserializeUint32(in, uwidth);
        byteArraySerializer.deserializeUint32(in, uheight);
        byteArraySerializer.deserializeUint32(in, uquality);
        byteArraySerializer.deserializeUint32(in, uflat);
        Frame<8>::coord_t width = static_cast<Frame<8>::coord_t>(uwidth);
        Frame<8>::coord_t height = static_cast<Frame<8>::coord_t>(uheight);
        uint8_t quality = uquality & 0xFF;
        bool flat = uflat != 0;
        Frame<8> current(width, height, 16);
        Frame<8> previous(width, height, 16);
        vector<uint8_t> uncompressed(current.getWidth() * current.getHeight());
        vector<uint8_t> precompressed(current.getAlignedWidth() * current.getAlignedHeight() * Precompressor::MAX_BYTES);
        vector<uint8_t> compressed(precompressed.size());
        ZlibDecompress zlibDecompress;
        const Frame<8>::coord_t *zigZagScan = ZigZagScan<8>::getScan();
        Precompressor precompressor(&precompressed[0]);
        Quantization quantization(flat, quality);
        DCT dct;
        Predictor predictor;
        Normalize normalize;
        if(!silent)
        {
            cerr << width << "x" << height << "@"; 
        }
        cerr << static_cast<int>(quality) << (flat ? " flat\n" : " non-flat\n");
        if(!silent)
        {
            cerr << "Decompressing... ";   
        }
#ifdef MEASURE_TIME
        clock_t begin = clock();
#endif
        for(unsigned count = 0; ; count++)
        {
            if(!silent)
            {
                cerr << count << " ";
            }
            uint32_t compressedSize = byteArraySerializer.deserializeByteArray(in, &compressed[0], compressed.size());
            if(compressedSize == 0)
            {
                break;
            }
            zlibDecompress(&compressed[0], &precompressed[0], compressedSize, precompressed.size());
            swap(current, previous);
            if(precompressor.applyReverse(current.scanningBegin(zigZagScan), current.scanningEnd())
                != precompressed.size() / Precompressor::MAX_BYTES)
            {
                break;
            }
            quantization.applyReverse(current.horizontalBegin(), current.horizontalEnd());
            dct.applyReverse(current.horizontalBegin(), current.horizontalEnd());
            dct.applyReverse(current.verticalBegin(), current.verticalEnd());
            predictor.applyReverse(current.horizontalBegin(), current.horizontalEnd(),
                                   previous.horizontalBegin());
            normalize(current.horizontalBegin(), current.horizontalEnd());
            copy(current.begin(), current.end(), uncompressed.begin());
            byteArraySerializer.serializeByteArray(&uncompressed[0], uncompressed.size(), out, false);
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