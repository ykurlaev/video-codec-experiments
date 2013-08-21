#include "bytearrayserializer.h"
#include <stdexcept>

namespace Codec
{

using std::feof;
using std::fread;
using std::fwrite;
using std::runtime_error;

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
        throw runtime_error("Can't read input stream");
    }
    uint32 = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
    return true;
}

void ByteArraySerializer::serializeUint32(uint32_t uint32, FILE *file)
{
    uint8_t bytes[4] = {static_cast<uint8_t>(uint32 & 0xFF),
                        static_cast<uint8_t>((uint32 >> 8) & 0xFF),
                        static_cast<uint8_t>((uint32 >> 16) & 0xFF),
                        static_cast<uint8_t>((uint32 >> 24) & 0xFF)};
    size_t n = fwrite(bytes, 1, 4, file);
    if(n != 4)
    {
        throw runtime_error("Can't write output stream");
    }
}

uint32_t ByteArraySerializer::deserializeByteArray(FILE *file, uint8_t *bytes, uint32_t maxSize, bool readSize)
{
    uint32_t size = maxSize;
    if(readSize)
    {
        if(!deserializeUint32(file, size))
        {
            return 0;
        }
    }
    if(size > maxSize)
    {
        throw runtime_error("Invalid format");
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
            throw runtime_error("Can't read input stream");
        }
    }
    return size;
}

void ByteArraySerializer::serializeByteArray(const uint8_t *bytes, uint32_t size, FILE *file, bool writeSize)
{
    if(writeSize)
    {
        serializeUint32(size, file);
    }
    size_t n = fwrite(bytes, 1, size, file);
    if(n != size)
    {
        throw runtime_error("Can't write output stream");
    }
}

ByteArraySerializer::~ByteArraySerializer()
{}

}
