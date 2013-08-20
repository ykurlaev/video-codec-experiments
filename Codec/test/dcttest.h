#ifndef CODEC_DCTTEST_H
#define CODEC_DCTTEST_H

#include "test.h"

class DCTTest : public Test
{
    public:
        virtual void operator()();
    private:
        static bool testDct();
};

#endif //CODEC_DCTTEST_H
