#ifndef CODEC_FRAMESERIALIZER_H
#define CODEC_FRAMESERIALIZER_H

#include <cstdio>
#include <vector>
#include "../region/region.h"
#include "../util.h"

namespace Codec
{

class FrameSerializer
{
    public:
        FrameSerializer(Region::coord_t width, Region::coord_t height);
        virtual bool deserialize(FILE *file, Region &region);
        virtual void serialize(const Region &region, FILE *file);
        virtual ~FrameSerializer();
    private:
        Region::coord_t m_width;
        Region::coord_t m_height;
        std::vector<uint8_t> m_buffer;
};

}

#endif //CODEC_FRAMESERIALIZER_H