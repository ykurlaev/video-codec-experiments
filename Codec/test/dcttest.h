#ifndef CODEC_DCTTEST_H
#define CODEC_DCTTEST_H

#include "test.h"

class DCTTest : public Test
{
    public:
        virtual void operator()();
    private:
        static bool testDctRandom();
        static bool testDctBasisForward();
        static bool testDctBasisReverse();
};

#endif //CODEC_DCTTEST_H
