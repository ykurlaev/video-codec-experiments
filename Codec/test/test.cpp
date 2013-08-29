#include "test.h"
#include <stdexcept>

using std::runtime_error;

void Test::fail(const char *why)
{
    throw runtime_error(why);
}

Test::~Test()
{}
