#include "encode.h"
#ifdef MEASURE_TIME
#include <ctime>
#endif
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <exception>
#include "../region/region.h"
#include "../bytearrayserializer.h"
#include "../frameserializer.h"
#include "dct.h"
#include "quantize.h"
#include "reorder.h"
#include "precompress.h"
#include "zlibcompress.h"

namespace Codec
{

using std::vector;
using std::cerr;
using std::fstream;
using std::ios_base;
using std::istringstream;
using std::exception;

////###DEBUG
//extern long long max_dc;
//extern long long max_ac;
//extern long long min_dc;
//extern long long min_ac;
//extern long long sum_ac;
//extern long long sum_dc;
//extern long long acc;
//extern long long dcc;
////###

int encode(int argc, char *argv[])
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
    Region::coord_t width;
    Region::coord_t height;
    istringstream(argv[1]) >> width;
    istringstream(argv[2]) >> height;
    uint32_t quality = 50;
    if(argc > 4)
    {
        istringstream(argv[4]) >> quality;
    }
    uint32_t flat = 0;
    if(argc > 5)
    {
        istringstream(argv[5]) >> flat;
    }
    bool silent = false;
    if(argc > 6 && strncmp(argv[6], "-q", 3))
    {
        silent = true;
    }
    ByteArraySerializer byteArraySerializer;
    byteArraySerializer.serializeUint32(width, out);
    byteArraySerializer.serializeUint32(height, out);
    byteArraySerializer.serializeUint32(quality, out);
    byteArraySerializer.serializeUint32(flat, out);
    Region::coord_t alignedWidth = (((width + 7) / 8) * 8);
    Region::coord_t alignedHeight = (((height + 7) / 8) * 8);
    std::vector<char> inBuffer(alignedWidth * alignedHeight * 2);
    std::vector<char> tmpBuffer(alignedWidth * alignedHeight * 2);
    std::vector<char> outBuffer(alignedWidth * alignedHeight * 2);
    FrameSerializer frameSerializer(width, height);
    DCT dct;
    Quantize quantize(flat != 0, static_cast<uint8_t>(quality));
    Reorder<int16_t> reorder;
    Precompress precompress;
    ZlibCompress zlibCompress;
    RegionBuffer current(alignedWidth, alignedHeight);
    Region frame(&current, 0, 0, width, height);
    std::vector<Region> blocks;
    for(Region::coord_t y = 0; y < alignedHeight / 8; y++)
    {
        for(Region::coord_t x = 0; x < alignedWidth / 8; x++)
        {
            blocks.push_back(Region(&current, x * 8, y * 8, 8, 8));
        }
    }
    if(!silent)
    {
        cerr << "Compressing... ";
    }
#ifdef MEASURE_TIME
    clock_t begin = clock();
#endif
    for(unsigned count = 0; ; count++)
    {
        try
        {
            if(!silent)
            {
                cerr << count << " ";
            }
            current.clear();
            if(!frameSerializer.deserialize(in, frame))
            {
                break;
            }
            size_t i = 0;
            for(Region::coord_t y = 0; y < alignedHeight / 8; y++)
            {
                for(Region::coord_t x = 0; x < alignedWidth / 8; x++)
                {
                    dct(blocks[i]);
                    quantize(blocks[i]);
                    reorder(blocks[i], &inBuffer[i * 8 * 8 * 2]);
                    i++;
                }
            }
            uint32_t precompressedSize = precompress(&inBuffer[0], &tmpBuffer[0], inBuffer.size());
            uint32_t compressedSize = zlibCompress(&tmpBuffer[0], &outBuffer[0], precompressedSize, outBuffer.size());
            byteArraySerializer.serializeByteArray(&outBuffer[0], compressedSize, out);
        }
        catch(const exception &e)
        {
            cerr << "\nError: " << e.what() << "\n";
            return 2;
        }
    }
    if(!silent)
    {
        cerr << "Ok\n";
    }
    ////###DEBUG
    //cerr << " MinDC=" << min_dc << " MaxDC=" << max_dc << " AvgDC=" << sum_dc / static_cast<double>(dcc);
    //cerr << " MinAC=" << min_ac << " MaxAC=" << max_ac << " AvgAC=" << sum_ac / static_cast<double>(acc);
    ////###
#ifdef MEASURE_TIME
    clock_t end = clock();
    cerr << static_cast<double>(end - begin) / CLOCKS_PER_SEC << "\n";
#endif
    return 0;
}

}