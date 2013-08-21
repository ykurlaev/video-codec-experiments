namespace Codec
{

template <size_t N>
inline const typename Frame<N>::coord_t *ZigZagScan<N>::getScan()
{
    static ZigZagScan instance;
    return instance.m_scan;
}

template <size_t N>
inline ZigZagScan<N>::ZigZagScan()
{
    typename MakeSigned<typename Frame<N>::coord_t>::t x = 0, y = 0;
    int d = -1;
    for(size_t i = 0; i < (N * N + 1) / 2; i++)
    {
        m_scan[i] = y * N + x;
        m_scan[N * N - i - 1] = (N - y - 1) * N + (N - x - 1);
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
