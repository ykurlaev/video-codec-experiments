#ifndef CODEC_TEST_H
#define CODEC_TEST_H

#include <iostream>
#include <exception>

#define TEST_RUN(function) \
    runTest(function, #function)

class Test
{
    public:
        virtual void operator()() = 0;
        virtual ~Test();
    protected:
        template <typename F>
        void runTest(F f, const char *name);
        static void fail(const char *why);
};

template <typename F>
inline void Test::runTest(F f, const char *name)
{
    std::cerr << "Running test " << name << "... ";
    try
    {
        if(f())
        {
            std::cerr << "[PASS]\n";
        }
        else
        {
            std::cerr << "[FAIL]\n";
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << "[FAIL]: " << e.what() << "\n";
    }
}

#endif //CODEC_TEST_H
