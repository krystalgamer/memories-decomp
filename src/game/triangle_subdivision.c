#include "../types.h"
#include "../ygo_types.h"

/* Recursive triangle subdivision, written twice: once over s16 vertices
   (func_8006BCA4) and once over three-byte colour triplets (func_8006C120),
   followed by the three primitives the triplet version is built from.

   The two subdividers are the same algorithm - split at the three edge
   midpoints, recurse four ways, write twelve vertices at the last depth -
   and neither calls the other. They sit back to back in the image and the
   unit is bounded by unmatched assembly on both sides.

   The manifest used to record func_8006BCA4 at gcc_2_8_1_g8 and the triplet
   run at gcc_2_8_1_g0, which read as a profile boundary between them. It is
   not one: func_8006BCA4 touches no small data, and its object is identical
   under either profile, so the whole unit builds at gcc_2_8_1_g0.

   The primitives touch exactly indices 0, 1 and 2 of whatever they are
   handed, and none of them reads a fourth byte or a stride. The caller's
   buffer is a Triplet, which strides 4 - the fourth byte is never read or
   written by any of the four functions here, so it is carried, not used.

   func_8006C2FC has no caller anywhere in the tree. It stays with the other
   two because it is the third member of the same primitive set and sits
   between them in the image, not because a call edge places it. */
void func_8006C2FC(u8 *output, s32 first, s32 second, s32 third);
void func_8006C30C(u8 *destination, const u8 *source);
void func_8006C330(u8 *out, u8 *a, u8 *b);

/* Recursive triangle subdivision: splits the triangle (a, b, c) at its
 * three edge midpoints and, at the last depth, writes the four resulting
 * triangles (twelve 8-byte vertices) to out and returns the advanced
 * pointer; otherwise recurses into the four sub-triangles. The three sums
 * are written before the three halvings on purpose: per-component pairs
 * let gcc forward the first store and the function comes out short. */
s16 *func_8006BCA4(s16 *a, s16 *b, s16 *c, s16 *out, s32 depth) {
    s16 A[4];
    s16 B[4];
    s16 C[4];
    s16 m0[4];
    s16 m1[4];
    s16 m2[4];
    s32 t;

    t = *(u16 *)a;
    A[0] = t;
    A[1] = a[1];
    A[2] = a[2];
    B[0] = b[0];
    B[1] = b[1];
    B[2] = b[2];
    C[0] = c[0];
    C[1] = c[1];
    C[2] = c[2];
    m0[0] = b[0];
    m0[1] = b[1];
    m0[2] = b[2];
    m0[0] = m0[0] + c[0];
    m0[1] = m0[1] + c[1];
    m0[2] = m0[2] + c[2];
    m0[0] = m0[0] / 2;
    m0[1] = m0[1] / 2;
    m0[2] = m0[2] / 2;
    m1[0] = c[0];
    m1[1] = c[1];
    m1[2] = c[2];
    m1[0] = m1[0] + a[0];
    m1[1] = m1[1] + a[1];
    m1[2] = m1[2] + a[2];
    m1[0] = m1[0] / 2;
    m1[1] = m1[1] / 2;
    m1[2] = m1[2] / 2;
    m2[0] = a[0];
    m2[1] = a[1];
    m2[2] = a[2];
    m2[0] = m2[0] + b[0];
    m2[1] = m2[1] + b[1];
    m2[2] = m2[2] + b[2];
    m2[0] = m2[0] / 2;
    m2[1] = m2[1] / 2;
    m2[2] = m2[2] / 2;
    depth--;
    if (depth == 0) {
        out[0] = t;
        out[1] = A[1];
        out[2] = A[2];
        out[4] = m2[0];
        out[5] = m2[1];
        out[6] = m2[2];
        out[8] = m1[0];
        out[9] = m1[1];
        out[10] = m1[2];
        out[12] = B[0];
        out[13] = B[1];
        out[14] = B[2];
        out[16] = m0[0];
        out[17] = m0[1];
        out[18] = m0[2];
        out[20] = m2[0];
        out[21] = m2[1];
        out[22] = m2[2];
        out[24] = C[0];
        out[25] = C[1];
        out[26] = C[2];
        out[28] = m1[0];
        out[29] = m1[1];
        out[30] = m1[2];
        out[32] = m0[0];
        out[33] = m0[1];
        out[34] = m0[2];
        out[36] = m0[0];
        out[37] = m0[1];
        out[38] = m0[2];
        out[40] = m1[0];
        out[41] = m1[1];
        out[42] = m1[2];
        out[44] = m2[0];
        out[45] = m2[1];
        out[46] = m2[2];
        return out + 0x30;
    }
    out = func_8006BCA4(A, m2, m1, out, depth);
    out = func_8006BCA4(B, m0, m2, out, depth);
    out = func_8006BCA4(C, m1, m0, out, depth);
    out = func_8006BCA4(m0, m1, m2, out, depth);
    return out;
}

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
