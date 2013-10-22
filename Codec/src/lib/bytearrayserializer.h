#ifndef CODEC_BYTEARRAYSERIALIZER_H
#define CODEC_BYTEARRAYSERIALIZER_H

#include <cstdio>
#include "util.h"

namespace Codec
{

class ByteArraySerializer
{
    public:
        bool deserializeByteArray(FILE *file, uint8_t *bytes, uint32_t size);
        void serializeByteArray(const uint8_t *bytes, uint32_t size, FILE *file);
};

}

#endif //CODEC_BYTEARRAYREADER_H
