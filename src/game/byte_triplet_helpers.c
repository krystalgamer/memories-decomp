#include "../types.h"

void func_8006C2FC(unsigned char *output, int first, int second, int third)
{
    output[0] = first;
    output[1] = second;
    output[2] = third;
}

void func_8006C30C(unsigned char *destination, const unsigned char *source)
{
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
}

void func_8006C330(unsigned char *out, unsigned char *a, unsigned char *b)
{
    out[0]=(a[0]+b[0])/2; out[1]=(a[1]+b[1])/2; out[2]=(a[2]+b[2])/2;
}
