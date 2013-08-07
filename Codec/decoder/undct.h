#ifndef CODEC_UNDCT_H
#define CODEC_UNDCT_H

#include "../region/region.h"
#include "../util.h"

namespace Codec
{

class UnDCT
{
    public:
        UnDCT();
        virtual void operator()(Region &region);
        virtual ~UnDCT();
};

}

#endif //CODEC_UNDCT_H