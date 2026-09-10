/* Reclassified from matching_c (#3859). This was src/game/func_80032184.c,
 * byte-exact only under gcc_2_8_1_cc_g8_as_g0_split, whose compiler and
 * assembler disagree about small data (GCC -G8, MASPSX -G0). Under
 * gcc_2_8_1_g0, a single threshold, it is 105 of 105 instructions with 4
 * differing, opcode distance 0. The source below is the match, unchanged
 * apart from its include paths. */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../game/file_transfer.h"
#include "../unmatched.h"
#include "../game/graphics_frame.h"
#include "../ygo_types.h"

void func_80032184(FileTransferDescriptor *p, s32 mode) {
    s32 one;
    s32 w;
    s32 v;
    s32 t0;
    s32 t1;
    s32 c;
    s32 n;
    s32 u;
    s16 *g;
    s32 m;
    s32 m2v;

    one = 1;

    if (mode == one) {
        goto m1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto m0;
        }
        return;
    }
    if (mode == 2) {
        goto m2;
    }
    if (mode == 3) {
        goto m3;
    }
    return;

m0:
    m = 0xFFDDFFFF;
    *(s16 *)((u8 *)p + 0x30) = 0x300;
    *(s16 *)((u8 *)p + 0x32) = 0x100;
    *(s16 *)((u8 *)p + 4) = 0x40;
    t0 = D_8009B0F4;
    *(s16 *)((u8 *)p + 6) = 0x10;
    D_8009B0F4 = t0 & m;
    D_8009B0F4 = D_8009B0F4 | 0x10000;
    p->done = 2;
    v = D_8009B118;
    w = 0x20000;
    *(s32 *)&p->mode = w;
    p->value_08 = v;
    v += 0x800;
    p->value_0C = v;
    return;

m1:
    m = 0xFFDDFFFF;
    *(s16 *)((u8 *)p + 0x30) = 0x340;
    *(s16 *)((u8 *)p + 4) = 0x40;
    t1 = D_8009B0F4;
    *(s16 *)((u8 *)p + 6) = 0x10;
    D_8009B0F4 = t1 & m;
    u = D_8009B0F4;
    n = 0x10000;
    *(s16 *)((u8 *)p + 0x32) = 0;
    D_8009B0F4 = u | n;
    p->done = 2;
    v = D_8009B118;
    w = 0x4000;
    *(s32 *)&p->mode = w;
    p->value_08 = v;
    v += 0x800;
    p->value_0C = v;
    return;

m2:
    m2v = 0xFFDCFFFF;
    *(s32 *)&p->mode = 0x2000;
    D_8009B0F4 = D_8009B0F4 & m2v;
    p->value_0C = D_8009B118;
    p->value_08 = D_8009B118;
    p->done = 1;
    return;

m3:
    g = (s16 *)D_800E9D70;
    c = 0x100;
    g[0] = c;
    g[1] = 0xF0;
    g[2] = c;
    g[3] = 0x10;
    LoadImage2((RECT *)g, (u32 *)D_8009B118);
}
