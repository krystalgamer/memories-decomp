#include "../types.h"

typedef u8 Triplet[4];

extern void func_8006C30C(u8 *, const u8 *);
extern void func_8006C330(u8 *, u8 *, u8 *);

Triplet *func_8006C120(u8 *a, u8 *b, u8 *c, Triplet *out, s32 depth)
{
    Triplet *result;

    Triplet mid[6];

    func_8006C30C(mid[0], a);
    func_8006C30C(mid[1], b);
    func_8006C30C(mid[2], c);
    func_8006C330(mid[3], b, c);
    func_8006C330(mid[4], c, a);
    func_8006C330(mid[5], a, b);

    if (--depth != 0) {
        result = func_8006C120(mid[0], mid[5], mid[4], out, depth);
        result = func_8006C120(mid[1], mid[3], mid[5], result, depth);
        result = func_8006C120(mid[2], mid[4], mid[3], result, depth);
        result = func_8006C120(mid[3], mid[4], mid[5], result, depth);
    } else {
        func_8006C30C(out[0], mid[0]);
        func_8006C30C(out[1], mid[5]);
        func_8006C30C(out[2], mid[4]);
        func_8006C30C(out[3], mid[1]);
        func_8006C30C(out[4], mid[3]);
        func_8006C30C(out[5], mid[5]);
        func_8006C30C(out[6], mid[2]);
        func_8006C30C(out[7], mid[4]);
        func_8006C30C(out[8], mid[3]);
        func_8006C30C(out[9], mid[3]);
        func_8006C30C(out[10], mid[4]);
        func_8006C30C(out[11], mid[5]);
        result = out + 12;
    }
    return result;
}
