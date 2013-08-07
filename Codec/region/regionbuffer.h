#ifndef CODEC_REGIONBUFFER_H
#define CODEC_REGIONBUFFER_H

#include <vector>
#include "../util.h"

namespace Codec
{

class RegionBuffer
{
    public:
        typedef uint32_t coord_t;
        typedef int32_t scoord_t;
        typedef int data_t;
        RegionBuffer(coord_t width, coord_t height, coord_t length = 1);
        RegionBuffer(const RegionBuffer &other);
        RegionBuffer &operator=(const RegionBuffer &other);
        coord_t getWidth() const;
        coord_t getHeight() const;
        coord_t getLength() const;
        const data_t &operator()(coord_t x, coord_t y, coord_t t) const;
        data_t &operator()(coord_t x, coord_t y, coord_t t);
        void clear();
    private:
        coord_t m_width;
        coord_t m_height;
        coord_t m_length;
        std::vector<data_t> m_data;
        friend void swap(RegionBuffer &first, RegionBuffer &second);
};

void swap(RegionBuffer &first, RegionBuffer &second);

}

#include "regionbuffer.inl"

#endif //CODEC_REGIONBUFFER_H