#ifndef CODEC_FRAMEREADER_H
#define CODEC_FRAMEREADER_H

#include <cstdio>

namespace Codec
{

class FrameReader
{
    public:
        enum Format { Y8, YUV888, YUV12 };
        FrameReader(FILE *file, Format format);
};

}

#endif
