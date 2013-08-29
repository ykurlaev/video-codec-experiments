#ifndef CODEC_ZIGZAGSCANTEST_H
#define CODEC_ZIGZAGSCANTEST_H

#include "test.h"

class ZigZagScanTest : public Test
{
    public:
        virtual void operator()();
    private:
        static bool testZigZagScan();
};


#endif //CODEC_ZIGZAGSCAN_H
