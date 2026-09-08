#include "../../../../src/types.h"

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

extern Record D_800F2C40[];
extern s32 D_800F56F0[];
extern s16 D_8009B47A;
extern u8 D_8009AF98;
extern u8 D_8009AF99;

extern s32 rcos(s32);
extern s32 rsin(s32);
extern s32 SquareRoot0(s32);
extern void Model_UpdateViewMetrics(s32);

s32 func_80051350(s32 mode, s32 min_extent, s32 depth)
{
    s32 e3[2];
    s32 e0[2];
    s32 e1[2];
    s32 e2[2];
    s32 dx[2];
    s32 dy[2];
    s32 dz[2];
    s32 dist[2];
    s32 moved;
    s32 hits;
    s32 i;
    s32 ox;
    s32 oz;
    s32 ref;

    ox = rcos(D_8009B47A + 0x800) * min_extent / 4096;
    oz = rsin(D_8009B47A + 0x800) * min_extent / 4096;

    e3[0] = (s16)D_800F2C40[0].h_DCE / 2;
    if (e3[0] < min_extent) {
        e3[0] = min_extent;
    }
    e3[1] = (s16)D_800F2C40[1].h_DCE / 2;
    if (e3[1] < min_extent) {
        e3[1] = min_extent;
    }
    e0[0] = (s16)D_800F2C40[0].h_DC8 / 2;
    if (e0[0] < min_extent) {
        e0[0] = min_extent;
    }
    e0[1] = (s16)D_800F2C40[1].h_DC8 / 2;
    if (e0[1] < min_extent) {
        e0[1] = min_extent;
    }
    e1[0] = (s16)D_800F2C40[0].h_DCA / 2;
    if (e1[0] < min_extent) {
        e1[0] = min_extent;
    }
    e1[1] = (s16)D_800F2C40[1].h_DCA / 2;
    if (e1[1] < min_extent) {
        e1[1] = min_extent;
    }
    e2[0] = (s16)D_800F2C40[0].h_DCC / 2;
    if (e2[0] < min_extent) {
        e2[0] = min_extent;
    }
    e2[1] = (s16)D_800F2C40[1].h_DCC / 2;
    if (e2[1] < min_extent) {
        e2[1] = min_extent;
    }

    ref = D_800F56F0[0] + ox;
    dx[0] = ref - D_800F2C40[0].s_DD0;
    dx[1] = ref - D_800F2C40[1].s_DD0;
    ref = D_800F56F0[1];
    dy[0] = ref - D_800F2C40[0].s_DD2;
    dy[1] = ref - D_800F2C40[1].s_DD2;
    ref = D_800F56F0[2] + oz;
    dz[0] = ref - D_800F2C40[0].s_DD4;
    dz[1] = ref - D_800F2C40[1].s_DD4;

    dist[0] = SquareRoot0(dx[0] * dx[0] + dz[0] * dz[0]);
    dist[1] = SquareRoot0(dx[1] * dx[1] + dz[1] * dz[1]);

    moved = 0;
    hits = 0;
    if (D_800F2C40[0].b_E1F != 0) {
        if (D_800F2C40[0].packed.b.hi >= 2) {
            if ((D_800F2C40[0].packed.word & 0xFFFFFF) == 0) {
                dist[0] = -1;
            }
        }
    } else {
        dist[0] = -1;
    }
    if (D_800F2C40[1].b_E1F != 0) {
        if (D_800F2C40[1].packed.b.hi >= 2) {
            if ((D_800F2C40[1].packed.word & 0xFFFFFF) == 0) {
                dist[1] = -1;
            }
        }
    } else {
        dist[1] = -1;
    }

    depth = depth + 1;
    for (i = 0; i < 2; i++) {
        s32 limit;
        s32 d;
        s32 v;

        limit = e2[i];
        if (limit < e0[i]) {
            limit = e0[i];
        }
        if (limit < e3[i]) {
            limit = e3[i];
        }
        v = dy[i];
        if (v < 0) {
            v = -v;
        }
        if (e1[i] < v) {
            continue;
        }
        d = dist[i];
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
            px = dx[i] * scale / 4096;
            if (dx[i] > 0) {
                px = -px;
            }
            pz = dz[i] * scale / 4096;
            if (dz[i] > 0) {
                pz = -pz;
            }
            D_800F56F0[0] = D_800F56F0[0] + px;
            D_800F2C40[i].s_DD4 = D_800F2C40[i].s_DD4 + pz;
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
