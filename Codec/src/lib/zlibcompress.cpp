#include "zlibcompress.h"
#include <stdexcept>

namespace Codec
{

using std::runtime_error;

ZlibCompress::ZlibCompress(int level)
{
    m_zStream.zalloc = Z_NULL;
    m_zStream.zfree = Z_NULL;
    m_zStream.opaque = Z_NULL;
    deflateInit(&m_zStream, level);
}

void ZlibCompress::setOutput(uint8_t *ptr, size_t size)
{
    m_zStream.next_out = ptr;
    m_zStream.avail_out = size;
    m_maxOutputSize = size;
}

void ZlibCompress::operator()(uint8_t *ptr, size_t size)
{
    m_zStream.next_in = ptr;
    m_zStream.avail_in = size;
    deflate(&m_zStream, Z_NO_FLUSH);
    if(m_zStream.msg != NULL)
    {
        throw runtime_error(m_zStream.msg);
    }
}

size_t ZlibCompress::getOutputSize()
{
    m_zStream.next_in = NULL;
    m_zStream.avail_in = 0;
    deflate(&m_zStream, Z_FULL_FLUSH);
    return m_maxOutputSize - m_zStream.avail_out;
}

ZlibCompress::~ZlibCompress()
{
    deflateEnd(&m_zStream);
}

}
