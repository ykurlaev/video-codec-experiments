#include "bytearrayserializer.h"
#include <stdexcept>

namespace Codec
{

using std::feof;
using std::fread;
using std::fwrite;
using std::runtime_error;

bool ByteArraySerializer::deserializeByteArray(FILE *file, uint8_t *bytes, uint32_t size)
{
    size_t n = fread(bytes, 1, size, file);
    if(feof(file))
    {
        return false;
    }
    if(n != size)
    {
        throw runtime_error("Can't read input stream");
    }
    return true;
}

void ByteArraySerializer::serializeByteArray(const uint8_t *bytes, uint32_t size, FILE *file)
{
    size_t n = fwrite(bytes, 1, size, file);
    if(n != size)
    {
        throw runtime_error("Can't write output stream");
    }
}

}
