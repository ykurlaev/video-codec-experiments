namespace Codec
{

template <size_t MIN_N, size_t MAX_N>
inline const coord_t *ZigZagScan<MIN_N, MAX_N>::getScan()
{
    static ZigZagScan instance;
    return instance.m_scan;
}

template <size_t MIN_N, size_t MAX_N>
inline ZigZagScan<MIN_N, MAX_N>::ZigZagScan()
{
    for(coord_t block = 0; block < (MAX_N * MAX_N) / (MIN_N * MIN_N); block++)
    {
        coord_t blockX = 0;
        coord_t blockY = 0;
        for(uint32_t size = MAX_N / 2; size >= MIN_N; size /= 2)
        {
            blockX += (block % 2) * size;
            blockY += (block / 2) * size;
        }
        scoord_t x = 0, y = 0;
        int d = -1;
        for(size_t i = 0; i < (MIN_N * MIN_N + 1) / 2; i++)
        {
            m_scan[block * MIN_N * MIN_N + i] = (blockY + y) * MAX_N + blockX + x;
            m_scan[block * MIN_N * MIN_N + MIN_N * MIN_N - i - 1]
                = (blockY + (MIN_N - 1 - y)) * MAX_N + (blockX + (MIN_N - 1 - x));
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

}
