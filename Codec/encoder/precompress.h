#ifndef CODEC_PRECOMPRESS_H
#define CODEC_PRECOMPRESS_H

#include <cstddef>
#include "../util.h"

namespace Codec
{

class Precompress
{
    public:
        Precompress();
        virtual size_t operator()(const char *from, char *to, size_t size);
        virtual ~Precompress();
};

}

#endif //CODEC_PRECOMPRESS_H