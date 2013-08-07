#ifndef CODEC_DCT_H
#define CODEC_DCT_H

#include "../region/region.h"
#include "../util.h"

namespace Codec
{

class DCT
{
    public:
	    DCT();
	    virtual void operator()(Region &region);
	    virtual ~DCT();
};

}

#endif //CODEC_DCT_H