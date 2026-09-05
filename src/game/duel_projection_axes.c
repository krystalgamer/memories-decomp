#include "../types.h"

extern long RotAverage3();
extern void func_8005B260(int, int, int, int);

void func_80029684(
    int arg0,
    int arg1,
    s16 *dst,
    int *control,
    int origin,
    int span
)
{
    int reduced;

    dst[0] = origin + span;
    dst[4] = origin + (span * 3) / 4;
    dst[8] = origin;
    RotAverage3(
        dst,
        dst + 4,
        dst + 8,
        arg0 + 8,
        arg0 + 16,
        arg0 + 24,
        control,
        control + 1
    );
    if (control[1] < 0) {
        reduced = span / 2;
        dst[0] = origin + reduced;
        dst[4] = origin + (reduced * 3) / 4;
        RotAverage3(
            dst,
            dst + 4,
            dst + 8,
            arg0 + 8,
            arg0 + 16,
            arg0 + 24,
            control,
            control + 1
        );
    }
    func_8005B260(arg0, arg1, 1, 1);
}

void func_800297DC(
    int arg0,
    int arg1,
    s16 *dst,
    int *control,
    int origin,
    int span
)
{
    int reduced;

    dst[2] = origin + span;
    dst[6] = origin + (span * 3) / 4;
    dst[10] = origin;
    RotAverage3(
        dst,
        dst + 4,
        dst + 8,
        arg0 + 8,
        arg0 + 16,
        arg0 + 24,
        control,
        control + 1
    );
    if (control[1] < 0) {
        reduced = span / 2;
        dst[2] = origin + reduced;
        dst[6] = origin + (reduced * 3) / 4;
        RotAverage3(
            dst,
            dst + 4,
            dst + 8,
            arg0 + 8,
            arg0 + 16,
            arg0 + 24,
            control,
            control + 1
        );
    }
    func_8005B260(arg0, arg1, 1, 1);
}
