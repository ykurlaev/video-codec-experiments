#include "decode.h"
#include <cstdio>
#ifdef MEASURE_TIME
#include <ctime>
#endif
#include <vector>
#include <iostream>
#include <exception>
#include "../region/region.h"
#include "../bytearrayserializer.h"
#include "zlibdecompress.h"
#include "unprecompress.h"
#include "unreorder.h"
#include "dequantize.h"
#include "undct.h"
#include "../frameserializer.h"

namespace Codec
{

using std::cerr;
using std::exception;

int decode(int argc, char *argv[])
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
    if(argc > 3 && strncmp(argv[3], "-q", 3))
    {
        silent = true;
    }
    Region::coord_t width;
    Region::coord_t height;
    uint32_t quality;
    uint32_t flat;
    ByteArraySerializer byteArraySerializer;
    byteArraySerializer.deserializeUint32(in, width);
    byteArraySerializer.deserializeUint32(in, height);
    byteArraySerializer.deserializeUint32(in, quality);
    byteArraySerializer.deserializeUint32(in, flat);
    Region::coord_t alignedWidth = (((width + 7) / 8) * 8);
    Region::coord_t alignedHeight = (((height + 7) / 8) * 8);
    std::vector<char> inBuffer(alignedWidth * alignedHeight * 2);
    std::vector<char> tmpBuffer(alignedWidth * alignedHeight * 2);
    std::vector<char> outBuffer(alignedWidth * alignedHeight * 2);
    ZlibDecompress zlibDecompress;
    UnPrecompress unPrecompress;
    Unreorder<int16_t> unreorder;
    Dequantize dequantize(flat != 0, static_cast<uint8_t>(quality));
    UnDCT undct;
    FrameSerializer frameSerializer(width, height);
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
        cerr << width << "x" << height << "@"; 
    }
    cerr << quality << ((flat == 0) ? " non-flat\n" : " flat\n");
    if(!silent)
    {
        cerr << "Decompressing... ";   
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
            uint32_t compressedSize = byteArraySerializer.deserializeByteArray(in, &inBuffer[0], inBuffer.size());
            if(compressedSize == 0)
            {
                break;
            }
            uint32_t precompressedSize = zlibDecompress(&inBuffer[0], &tmpBuffer[0], compressedSize, tmpBuffer.size());
            unPrecompress(&tmpBuffer[0], &outBuffer[0], precompressedSize);
            size_t i = 0;
            current.clear();
            for(Region::coord_t y = 0; y < alignedHeight / 8; y++)
            {
                for(Region::coord_t x = 0; x < alignedWidth / 8; x++)
                {
                    unreorder(&outBuffer[i * 8 * 8 * 2], blocks[i]);
                    dequantize(blocks[i]);
                    undct(blocks[i]);
                    i++;
                }
            }
            frameSerializer.serialize(frame, out);
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
#ifdef MEASURE_TIME
    clock_t end = clock();
    cerr << static_cast<double>(end - begin) / CLOCKS_PER_SEC << "\n";
#endif
    return 0;
}

}