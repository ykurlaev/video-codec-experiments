#include <vector>
#include "dcttest.h"
#include "zigzagscantest.h"
#include "frametest.h"

using std::vector;

int main()
{
    vector<Test *> tests;
    tests.push_back(new DCTTest);
    tests.push_back(new ZigZagScanTest);
    tests.push_back(new FrameTest);
    Test::runTests(tests);
    return 0;
}
