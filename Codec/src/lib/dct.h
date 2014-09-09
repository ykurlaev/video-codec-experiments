#ifndef CODEC_DCT_H
#define CODEC_DCT_H

#include "util.h"

namespace Codec
{

class DCT
{
    public:
        template <typename T>
	    void applyForward(T &a, T &b, T &c, T &d, T &e, T &f, T &g, T &h);
        template <typename T>
	    void applyReverse(T &a, T &b, T &c, T &d, T &e, T &f, T &g, T &h);
        template <typename Iterator>
        void applyForward(Iterator begin, Iterator end);
        template <typename Iterator>
        void applyReverse(Iterator begin, Iterator end);
};

}

#include "dct.inl"

#endif //CODEC_DCT_H
