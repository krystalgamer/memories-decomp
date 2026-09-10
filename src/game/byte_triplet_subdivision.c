#include "../types.h"

/* Recursive triangle subdivision over three-byte colour triplets, and the
   three primitives it is built from.

   This is the u8 twin of func_8006BCA4, which does the same subdivision over
   s16 vertices. The two cannot share a unit: func_8006BCA4 compiles at
   gcc_2_8_1_g8 and this run at gcc_2_8_1_g0, and it is that profile change,
   at 0x8006C120, that bounds this unit below.

   The primitives touch exactly indices 0, 1 and 2 of whatever they are
   handed, and none of them reads a fourth byte or a stride. The caller's
   buffer is a Triplet, which strides 4 - the fourth byte is never read or
   written by any of the four functions here, so it is carried, not used.

   func_8006C2FC has no caller anywhere in the tree. It stays with the other
   two because it is the third member of the same primitive set and sits
   between them in the image, not because a call edge places it. */
typedef u8 Triplet[4];

void func_8006C2FC(u8 *output, s32 first, s32 second, s32 third);
void func_8006C30C(u8 *destination, const u8 *source);
void func_8006C330(u8 *out, u8 *a, u8 *b);

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

void func_8006C2FC(u8 *output, s32 first, s32 second, s32 third)
{
    output[0] = first;
    output[1] = second;
    output[2] = third;
}

void func_8006C30C(u8 *destination, const u8 *source)
{
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
}

void func_8006C330(u8 *out, u8 *a, u8 *b)
{
    out[0]=(a[0]+b[0])/2; out[1]=(a[1]+b[1])/2; out[2]=(a[2]+b[2])/2;
}
