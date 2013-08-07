#ifndef CODEC_UNPRECOMPRESS_H
#define CODEC_UNPRECOMPRESS_H

#include <cstddef>
#include "../util.h"

namespace Codec
{

class UnPrecompress
{
    public:
        UnPrecompress();
        virtual void operator()(const char *from, char *to, size_t size);
        virtual ~UnPrecompress();
};

}

#endif //CODEC_PRECOMPRESS_H