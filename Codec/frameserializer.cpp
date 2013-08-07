#include "frameserializer.h"
#include <algorithm>
#include <exception>

namespace Codec
{

using std::vector;
using std::copy;
using std::exception;

FrameSerializer::FrameSerializer(Region::coord_t width, Region::coord_t height)
    : m_width(width), m_height(height), m_buffer(width * height)
{}

bool FrameSerializer::deserialize(FILE *file, Region &region)
{
    if(region.getWidth() != m_width || region.getHeight() != m_height)
    {
        throw exception("Invalid region size");
    }
    size_t n = fread(&m_buffer[0], 1, m_width * m_height, file);
    if(feof(file))
    {
        return false;
    }
    if(n != m_width * m_height)
    {
        throw exception("Can't read input stream");
    }
    vector<uint8_t>::const_iterator from = m_buffer.begin();
    Region::iterator to = region.begin();
    for(; from != m_buffer.end(); ++from, ++to)
    {
        *to = static_cast<Region::data_t>(*from);
    }
    return true;
}

void FrameSerializer::serialize(const Region &region, FILE *file)
{
    if(region.getWidth() != m_width || region.getHeight() != m_height)
    {
        throw exception("Invalid region size");
    }
    Region::const_iterator from = region.begin();
    vector<uint8_t>::iterator to = m_buffer.begin();
    for(; from != region.end(); ++from, ++to)
    {
        *to = static_cast<uint8_t>(*from);
    }
    size_t n = fwrite(&m_buffer[0], 1, m_width * m_height, file);
    if(n != m_width * m_height)
    {
        throw exception("Can't write output stream");
    }
}

FrameSerializer::~FrameSerializer()
{}

}