#include <cstdlib>

namespace Codec
{

template <typename Iterator1, typename Iterator2>
uint32_t FindSAD::operator()(Iterator1 begin, Iterator1 end, Iterator2 second)
{
    uint32_t result = 0;
    for(; begin != end; ++begin, ++second)
    {
        result += std::abs(*begin - *second);
    }
    return result;
}

}
