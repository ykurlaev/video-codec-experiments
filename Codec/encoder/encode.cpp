#include "encode.h"
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
#include "../frame.h"
#include "../bytearrayserializer.h"
#include "../zigzagscan.h"
#include "dct.h"
#include "quantize.h"
#include "precompress.h"
#include "zlibcompress.h"

namespace Codec
{

using std::vector;
using std::cerr;
using std::istringstream;
using std::copy;
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
        Frame<8> current(width, height, 16);
        vector<uint8_t> uncompressed(current.getWidth() * current.getHeight());
        vector<uint8_t> precompressed(current.getAlignedWidth() * current.getAlignedHeight() * Precompress::MAX_BYTES);
        vector<uint8_t> compressed(precompressed.size());
        DCT dct;
        Quantize quantize(flat, quality, ((1 << 8) * 8 * 8) / 2);
        const Frame<8>::coord_t *zigZagScan = ZigZagScan<8>::getScan();
        Precompress precompress(&precompressed[0]);
        ZlibCompress zlibCompress;
        if(!silent)
        {
            cerr << "Compressing... ";
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
            current.clear();
            if(byteArraySerializer.deserializeByteArray(in, &uncompressed[0], uncompressed.size(), false)
               != uncompressed.size())
            {
                break;
            }
            copy(uncompressed.begin(), uncompressed.end(), current.begin());
            current.applyHorizontal(dct);
            current.applyVertical(dct);
            current.apply(quantize);
            current.applyScanning(precompress, zigZagScan);
            uint32_t compressedSize = zlibCompress(&precompressed[0], &compressed[0],
                                                   precompress.getOutputSize(), compressed.size());
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