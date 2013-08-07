#ifndef CODEC_REORDER_H
#define CODEC_REORDER_H

#include "../region/region.h"
#include "../util.h"

namespace Codec
{

template <typename T = int16_t>
class Reorder
{
    public:
        Reorder();
        virtual void operator()(const Region &region, char *bytes);
        virtual ~Reorder();
};

}

#include "reorder.inl"

#endif //CODEC_REORDER_H