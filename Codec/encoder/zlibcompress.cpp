#include "zlibcompress.h"
#include <exception>

namespace Codec
{

using std::exception;

ZlibCompress::ZlibCompress(int level)
    : m_level(level)
{
    m_zStream.zalloc = Z_NULL;
    m_zStream.zfree = Z_NULL;
    m_zStream.opaque = Z_NULL;
    deflateInit(&m_zStream, m_level);
}

size_t ZlibCompress::operator()(char *from, char *to, size_t fromSize, size_t toSize)
{
    m_zStream.next_in = reinterpret_cast<Bytef *>(from);
    m_zStream.avail_in = fromSize;
    m_zStream.next_out = reinterpret_cast<Bytef *>(to);
    m_zStream.avail_out = toSize;
    deflate(&m_zStream, Z_SYNC_FLUSH);
    if(m_zStream.msg != NULL)
    {
        throw exception(m_zStream.msg);
    }
    return toSize - m_zStream.avail_out;
}

ZlibCompress::~ZlibCompress()
{
    deflateEnd(&m_zStream);
}

}