#ifndef CODEC_ZLIBCOMPRESS_H
#define CODEC_ZLIBCOMPRESS_H

#include <zlib.h>
#include "util.h"

namespace Codec
{

class ZlibCompress
{
    public:
        ZlibCompress(int level = Z_DEFAULT_COMPRESSION);
        void setOutput(uint8_t *ptr, size_t size);
        void operator()(uint8_t *ptr, size_t size);
        size_t getOutputSize();
        ~ZlibCompress();
    private:
        z_stream m_zStream;
        size_t m_maxOutputSize;
};

}

#endif //CODEC_ZLIBCOMPRESS_H
