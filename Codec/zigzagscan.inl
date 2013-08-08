namespace Codec
{

template <size_t N>
inline const typename Frame<N>::coord_t *ZigZagScan<N>::getScan()
{
    return sm_internal.m_scan;
}

template <size_t N>
typename ZigZagScan<N>::template ZigZagScanInternal<N> ZigZagScan<N>::sm_internal;

template <size_t N>
template <size_t M>
inline ZigZagScan<N>::ZigZagScanInternal<M>::ZigZagScanInternal()
{
    typename IntN<sizeof(typename Frame<M>::coord_t)>::s x = 0, y = 0;
    int d = -1;
    for(size_t i = 0; i < (M * M + 1) / 2; i++)
    {
        m_scan[i] = y * M + x;
        m_scan[M * M - i - 1] = (M - y - 1) * M + (M - x - 1);
        x += d;
        y -= d;
        if(x < 0)
        {
            x++;
            d = -d;
        }
        else if(y < 0)
        {
            y++;
            d = -d;
        }
    }
}

}