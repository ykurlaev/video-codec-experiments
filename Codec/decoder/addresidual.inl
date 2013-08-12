namespace Codec
{

template <typename Iterator1, typename Iterator2>
void AddResidual::operator()(Iterator1 begin, Iterator1 end, Iterator2 neighbor)
{
    for(; begin != end; ++begin, ++neighbor)
    {
        *begin += *neighbor;
    }
}

}