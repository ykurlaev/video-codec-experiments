#ifndef CODEC_ZLIBDECOMPRESS_H
#define CODEC_ZLIBDECOMPRESS_H

#include <zlib.h>
#include "util.h"

namespace Codec
{

class ZlibDecompress
{
    public:
        ZlibDecompress();
        size_t operator()(uint8_t *from, uint8_t *to, size_t fromSize, size_t toSize);
        ~ZlibDecompress();
    private:
        z_stream m_zStream;
};

}

#endif //CODEC_ZLIBDECOMPRESS_H
