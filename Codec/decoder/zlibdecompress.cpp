#include "zlibdecompress.h"
#include <stdexcept>

namespace Codec
{

using std::runtime_error;

ZlibDecompress::ZlibDecompress()
{
    m_zStream.zalloc = Z_NULL;
    m_zStream.zfree = Z_NULL;
    m_zStream.opaque = Z_NULL;
    inflateInit(&m_zStream);
}

size_t ZlibDecompress::operator()(uint8_t *from, uint8_t *to, size_t fromSize, size_t toSize)
{
    m_zStream.next_in = from;
    m_zStream.avail_in = fromSize;
    m_zStream.next_out = to;
    m_zStream.avail_out = toSize;
    inflate(&m_zStream, Z_SYNC_FLUSH);
    if(m_zStream.msg != NULL)
    {
        throw runtime_error(m_zStream.msg);
    }
    return toSize - m_zStream.avail_out;
}

ZlibDecompress::~ZlibDecompress()
{
    inflateEnd(&m_zStream);
}

}