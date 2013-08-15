#include "zlibcompress.h"
#include <stdexcept>

namespace Codec
{

using std::runtime_error;

ZlibCompress::ZlibCompress(int level)
    : m_level(level)
{
    m_zStream.zalloc = Z_NULL;
    m_zStream.zfree = Z_NULL;
    m_zStream.opaque = Z_NULL;
    deflateInit(&m_zStream, m_level);
}

size_t ZlibCompress::operator()(uint8_t *from, uint8_t *to, size_t fromSize, size_t toSize)
{
    m_zStream.next_in = from;
    m_zStream.avail_in = fromSize;
    m_zStream.next_out = to;
    m_zStream.avail_out = toSize;
    deflate(&m_zStream, Z_SYNC_FLUSH);
    if(m_zStream.msg != NULL)
    {
        throw runtime_error(m_zStream.msg);
    }
    return toSize - m_zStream.avail_out;
}

ZlibCompress::~ZlibCompress()
{
    deflateEnd(&m_zStream);
}

}