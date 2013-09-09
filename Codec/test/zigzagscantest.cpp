#include "zigzagscantest.h"
#include "../src/lib/zigzagscan.h"

using namespace Codec;

void ZigZagScanTest::operator()()
{
    TEST_RUN(testZigZagScan);
}

bool ZigZagScanTest::testZigZagScan()
{
    const Frame<>::coord_t *zigZagScan4 = ZigZagScan<4>::getScan();
    const Frame<>::coord_t desired4[] = { 0, 4, 1, 2, 5, 8, 12, 9, 6, 3, 7, 10, 13, 14, 11, 15 };
    for(int i = 0; i < 16; i++)
    {
        if(zigZagScan4[i] != desired4[i])
        {
            fail("ZigZagScan<4> fail");
        }
    }
    const Frame<>::coord_t *zigZagScan3 = ZigZagScan<3>::getScan();
    const Frame<>::coord_t desired3[] = { 0, 3, 1, 2, 4, 6, 7, 5, 8 };
    for(int i = 0; i < 9; i++)
    {
        if(zigZagScan3[i] != desired3[i])
        {
            fail("ZigZagScan<3> fail");
        }
    }
    const Frame<>::coord_t *zigZagScan2 = ZigZagScan<2>::getScan();
    const Frame<>::coord_t desired2[] = { 0, 2, 1, 3 };
    for(int i = 0; i < 4; i++)
    {
        if(zigZagScan2[i] != desired2[i])
        {
            fail("ZigZagScan<2> fail");
        }
    }
    return true;
}
