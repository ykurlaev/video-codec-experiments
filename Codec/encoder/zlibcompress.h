#ifndef CODEC_ZLIBCOMPRESS_H
#define CODEC_ZLIBCOMPRESS_H

#include <cstddef>
#include "../util.h"
#include <zlib.h>

namespace Codec
{

class ZlibCompress
{
    public:
        ZlibCompress(int level = Z_DEFAULT_COMPRESSION);
        virtual size_t operator()(uint8_t *from, uint8_t *to, size_t fromSize, size_t toSize);
        virtual ~ZlibCompress();
    private:
        int m_level;
        z_stream m_zStream;
};

}

#endif //CODEC_ZLIBCOMPRESS_H