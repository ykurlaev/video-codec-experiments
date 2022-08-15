#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>

int main(int argc, char *argv[])
{
    if(argc < 5)
    {
        fprintf(stderr, "Usage: %s <width> <height> <first file> <second file> [-v]\n",
                argc > 0 ? argv[0] : "<program>");
        return 1;
    }
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    FILE *first_file = fopen(argv[3], "rb");
    if(!first_file)
    {
        fprintf(stderr, "Error: Can't open %s\n", argv[3]);
        return 2;
    }
    FILE *second_file = fopen(argv[4], "rb");
    if(!second_file)
    {
        fprintf(stderr, "Error: Can't open %s\n", argv[4]);
        return 2;
    }
    bool verbose = (argc > 5) && !strncmp(argv[5], "-v", 3);
    int size = width * height;
    char *first = new char[size];
    char *second = new char[size];
    float max_msad = 0.f;
    float max_mse = 0.f;
    float min_psnr = 999999.f;
    float avg_msad = 0.f;
    float avg_mse = 0.f;
    float avg_psnr = 0.f;
    int c = 0;
    while(fread(first, 1, size, first_file) == size && fread(second, 1, size, second_file) == size)
    {
        float msad = 0.f;
        float mse = 0.f;
        for(int i = 0; i < size; i++)
        {
            float diff = static_cast<float>(abs(first[i] - second[i]));
            msad += diff;
            mse += diff * diff;
        }
        msad /= size;
        mse /= size;
        float psnr = 20.f * log10(255.f) - 10.f * log10(mse);
        if(verbose)
        {
            printf("%d %.2f,%.2f,%.2f\n", c + 1, msad, mse, psnr);
        }
        if(msad > max_msad)
        {
            max_msad = msad;
        }
        if(mse > max_mse)
        {
            max_mse = mse;
        }
        if(psnr < min_psnr)
        {
            min_psnr = psnr;
        }
        avg_msad += msad;
        avg_mse += mse;
        avg_psnr += psnr;
        c++;
    }
    avg_msad /= c;
    avg_mse /= c;
    avg_psnr /= c;
    printf("worst %.2f,%.2f,%.2f\n", max_msad, max_mse, min_psnr);
    printf("average %.2f,%.2f,%.2f\n", avg_msad, avg_mse, avg_psnr);
    fclose(first_file);
    fclose(second_file);
    delete[] first;
    delete[] second;
    return 0;
}
