#ifndef CODEC_BYTEARRAYSERIALIZER_H
#define CODEC_BYTEARRAYSERIALIZER_H

#include <cstdio>
#include "util.h"

namespace Codec
{

class ByteArraySerializer
{
    public:
        ByteArraySerializer();
        virtual bool deserializeUint32(FILE *file, uint32_t &uint32);
        virtual void serializeUint32(uint32_t uint32, FILE *file);
        virtual uint32_t deserializeByteArray(FILE *file, uint8_t *bytes, uint32_t maxSize, bool readSize = true);
        virtual void serializeByteArray(const uint8_t *bytes, uint32_t size, FILE *file, bool writeSize = true);
        virtual ~ByteArraySerializer();
};

}

#endif //CODEC_BYTEARRAYREADER_H
