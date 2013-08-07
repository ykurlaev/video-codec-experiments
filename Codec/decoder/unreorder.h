#ifndef CODEC_UNREORDER_H
#define CODEC_UNREORDER_H

#include "../region/region.h"
#include "../util.h"

namespace Codec
{

template <typename T = int16_t>
class Unreorder
{
    public:
        Unreorder();
        virtual void operator()(const char *region, Region &bytes);
        virtual ~Unreorder();
};

}

#include "unreorder.inl"

#endif //CODEC_UNREORDER_H