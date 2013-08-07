#ifndef CODEC_ZLIBDECOMPRESS_H
#define CODEC_ZLIBDECOMPRESS_H

#include <cstddef>
#include "../util.h"
#include <zlib.h>

namespace Codec
{

class ZlibDecompress
{
    public:
        ZlibDecompress();
        virtual size_t operator()(char *from, char *to, size_t fromSize, size_t toSize);
        virtual ~ZlibDecompress();
    private:
        z_stream m_zStream;
};

}

#endif //CODEC_ZLIBDECOMPRESS_H