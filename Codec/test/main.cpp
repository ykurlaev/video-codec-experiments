#include <vector>
#include "dcttest.h"

using std::vector;

int main()
{
    vector<Test *> tests;
    tests.push_back(new DCTTest);
    for(vector<Test *>::iterator it = tests.begin(); it != tests.end(); ++it)
    {
        (**it)();
        delete *it;
    }
    return 0;
}
