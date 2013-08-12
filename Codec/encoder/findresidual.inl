namespace Codec
{

template <typename Iterator1, typename Iterator2>
void FindResidual::operator()(Iterator1 begin, Iterator1 end, Iterator2 neighbor)
{
    for(; begin != end; ++begin, ++neighbor)
    {
        *begin -= *neighbor;
    }
}

}