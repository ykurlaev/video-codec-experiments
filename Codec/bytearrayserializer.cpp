#include "bytearrayserializer.h"
#include <exception>

namespace Codec
{

using std::exception;

ByteArraySerializer::ByteArraySerializer()
{}

bool ByteArraySerializer::deserializeUint32(FILE *file, uint32_t &uint32)
{
    uint8_t bytes[4] = {0};
    size_t n = fread(bytes, 1, 4, file);
    if(feof(file))
    {
        return false;
    }
    if(n != 4)
    {
        throw exception("Can't read input stream");
    }
    uint32 = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
    return true;
}

void ByteArraySerializer::serializeUint32(uint32_t uint32, FILE *file)
{
    uint8_t bytes[4] = {uint32 & 0xFF, (uint32 >> 8) & 0xFF,
                        (uint32 >> 16) & 0xFF, (uint32 >> 24) & 0xFF};
    size_t n = fwrite(bytes, 1, 4, file);
    if(n != 4)
    {
        throw exception("Can't write output stream");
    }
}

uint32_t ByteArraySerializer::deserializeByteArray(FILE *file, char *bytes, uint32_t maxSize)
{
    uint32_t size;
    if(!deserializeUint32(file, size))
    {
        return 0;
    }
    if(size > maxSize)
    {
        throw exception("Invalid format");
    }
    if(size > 0)
    {
        size_t n = fread(bytes, 1, size, file);
        if(feof(file))
        {
            return 0;
        }
        if(n != size)
        {
            throw exception("Can't read input stream");
        }
    }
    return size;
}

void ByteArraySerializer::serializeByteArray(char *bytes, uint32_t size, FILE *file)
{
    serializeUint32(size, file);
    size_t n = fwrite(bytes, 1, size, file);
    if(n != size)
    {
        throw exception("Can't write output stream");
    }
}

ByteArraySerializer::~ByteArraySerializer()
{}

}