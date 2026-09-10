/*
 * Recursively separates two model records when their projected distance is
 * below the largest paired half-extent. Current best under
 * gcc_2_8_1_g8_split: 450 instructions against 446, opcode multiset distance
 * 18, and 350 differing positions, with no hard register assignments.
 *
 * The full vector block, inverted negation guard, reference-vector push, and
 * depth-three retry are present. A scalar clamp temporary feeding one shared
 * two-word Pair temporary reproduces the target's layered assignments.
 *
 * Residual: six surplus nops, five missing moves, and isolated scheduling or
 * allocation differences in addiu, li, lw, sw, bgez, and j. Addressing already
 * matches. See notes/research/func-80051350-decode.md for the structural map.
 */
#include "../types.h"
#define MODEL_GRAPHICS_STATE_AF99_UNSIGNED
#include "../game/model_graphics_state.h"

typedef union {
    u32 word;
    struct {
        u8 lo[3];
        u8 hi;
    } b;
} Packed;

typedef struct {
    u8 pad_0000[0xDC0];
    Packed packed;
    u8 pad_0DC4[4];
    u16 h_DC8;
    u16 h_DCA;
    u16 h_DCC;
    u16 h_DCE;
    s16 s_DD0;
    s16 s_DD2;
    s16 s_DD4;
    u8 pad_0DD6[0xE1F - 0xDD6];
    u8 b_E1F;
} Record;

typedef struct {
    s32 v[2];
} Pair;

extern Record D_800F2C40[];
extern s32 D_800F56F0[];
extern s16 D_8009B47A;

extern s32 rcos(s32);
extern s32 rsin(s32);
extern s32 SquareRoot0(s32);
extern void Model_UpdateViewMetrics(s32);

s32 func_80051350(s32 mode, s32 min_extent, s32 depth)
{
    Pair e3;
    Pair e0;
    Pair e1;
    Pair e2;
    Pair dx;
    Pair dy;
    Pair dz;
    Pair dist;
    Pair t;
    s32 moved;
    s32 hits;
    s32 i;
    s32 ox;
    s32 oz;
    s32 ref;
    s32 v;

    ox = rcos(D_8009B47A + 0x800) * min_extent / 4096;
    oz = rsin(D_8009B47A + 0x800) * min_extent / 4096;

    v = (s16)D_800F2C40[0].h_DCE / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.v[0] = v;
    v = (s16)D_800F2C40[1].h_DCE / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.v[1] = v;
    e3 = t;
    v = (s16)D_800F2C40[0].h_DC8 / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.v[0] = v;
    v = (s16)D_800F2C40[1].h_DC8 / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.v[1] = v;
    e0 = t;
    v = (s16)D_800F2C40[0].h_DCA / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.v[0] = v;
    v = (s16)D_800F2C40[1].h_DCA / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.v[1] = v;
    e1 = t;
    v = (s16)D_800F2C40[0].h_DCC / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.v[0] = v;
    v = (s16)D_800F2C40[1].h_DCC / 2;
    if (v < min_extent) {
        v = min_extent;
    }
    t.v[1] = v;
    e2 = t;

    ref = D_800F56F0[0] + ox;
    t.v[0] = ref - D_800F2C40[0].s_DD0;
    t.v[1] = ref - D_800F2C40[1].s_DD0;
    dx = t;
    ref = D_800F56F0[1];
    t.v[0] = ref - D_800F2C40[0].s_DD2;
    t.v[1] = ref - D_800F2C40[1].s_DD2;
    dy = t;
    ref = D_800F56F0[2] + oz;
    t.v[0] = ref - D_800F2C40[0].s_DD4;
    t.v[1] = ref - D_800F2C40[1].s_DD4;
    dz = t;

    t.v[0] = SquareRoot0(dx.v[0] * dx.v[0] + dz.v[0] * dz.v[0]);
    t.v[1] = SquareRoot0(dx.v[1] * dx.v[1] + dz.v[1] * dz.v[1]);
    dist = t;

    moved = 0;
    hits = 0;
    if (D_800F2C40[0].b_E1F != 0) {
        if (D_800F2C40[0].packed.b.hi >= 2) {
            if ((D_800F2C40[0].packed.word & 0xFFFFFF) == 0) {
                dist.v[0] = -1;
            }
        }
    } else {
        dist.v[0] = -1;
    }
    if (D_800F2C40[1].b_E1F != 0) {
        if (D_800F2C40[1].packed.b.hi >= 2) {
            if ((D_800F2C40[1].packed.word & 0xFFFFFF) == 0) {
                dist.v[1] = -1;
            }
        }
    } else {
        dist.v[1] = -1;
    }

    depth = depth + 1;
    for (i = 0; i < 2; i++) {
        s32 limit;
        s32 d;
        s32 v;

        limit = e2.v[i];
        if (limit < e0.v[i]) {
            limit = e0.v[i];
        }
        if (limit < e3.v[i]) {
            limit = e3.v[i];
        }
        v = dy.v[i];
        if (v < 0) {
            v = -v;
        }
        if (e1.v[i] < v) {
            continue;
        }
        d = dist.v[i];
        if (d < 0) {
            continue;
        }
        if (d >= limit) {
            continue;
        }
        if (moved != 0) {
            hits = hits + 1;
            continue;
        }
        if (mode == 0) {
            moved = limit;
            continue;
        }
        {
            s32 scale;
            s32 px;
            s32 pz;

            scale = ((limit - d) << 12) / limit;
            px = dx.v[i] * scale / 4096;
            if (dx.v[i] <= 0) {
                px = -px;
            }
            pz = dz.v[i] * scale / 4096;
            if (dz.v[i] <= 0) {
                pz = -pz;
            }
            {
                s32 ax = D_800F56F0[3];
                s32 az = D_800F56F0[2];
                s32 bx = D_800F56F0[5];
                s32 cx = D_800F56F0[0];
                s32 ux = bx - az;
                s32 uz = cx - ax;
                s32 cross = ax * az - cx * bx;
                s32 len = SquareRoot0(ux * ux + uz * uz);
                s32 sd = 0;

                if (len != 0) {
                    sd = ((cx + px) * ux + (az + pz) * uz + cross) / len;
                }
                if (D_8009AF98 == 0) {
                    if (sd >= 0) {
                        D_8009AF99 = 1;
                    } else {
                        D_8009AF99 = -1;
                    }
                    D_8009AF98 = 0x1E;
                } else if (D_8009AF98 < 0xFF) {
                    D_8009AF98 = D_8009AF98 - 1;
                }
            }
            D_800F56F0[0] = D_800F56F0[0] + px;
            D_800F56F0[2] = D_800F56F0[2] + pz;
        }
    }

    if (mode != 0 && moved != 0) {
        Model_UpdateViewMetrics(0);
    }
    if (hits != 0 && mode != 0 && depth < 3) {
        func_80051350(mode, min_extent, depth);
    }
    return moved;
}
