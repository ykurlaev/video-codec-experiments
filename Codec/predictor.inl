namespace Codec
{

template <typename Iterator1, typename Iterator2>
void Predictor::applyForward(Iterator1 begin, Iterator1 end, Iterator2 neighbor)
{
    for(; begin != end; ++begin, ++neighbor)
    {
        *begin -= *neighbor;
    }
}

template <typename Iterator1, typename Iterator2>
void Predictor::applyReverse(Iterator1 begin, Iterator1 end, Iterator2 neighbor)
{
    for(; begin != end; ++begin, ++neighbor)
    {
        *begin += *neighbor;
    }
}

}