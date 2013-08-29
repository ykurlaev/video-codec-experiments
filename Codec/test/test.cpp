#include "test.h"
#include <stdexcept>

using std::vector;
using std::runtime_error;

void Test::fail(const char *why)
{
    throw runtime_error(why);
}

Test::~Test()
{}

void Test::runTests(vector<Test *> tests)
{
    for(vector<Test *>::iterator it = tests.begin(); it != tests.end(); ++it)
    {
        (**it)();
        delete *it;
    }
}