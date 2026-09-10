/*
 * Renders the 3D duel card through scratchpad matrices, four projected
 * corners, per-vertex lighting, and two ordering-table submissions. Current
 * best under gcc_2_8_1_g0: 386/386 instructions, opcode multiset distance 20,
 * and 341 differing words, with no hard register pin.
 *
 * Scratchpad pointers are initialized after the early guard, typed bases
 * preserve the retail address mix, the model Y rotation is intentionally
 * recomputed, and Blk8 assignments produce the unaligned corner copies. The
 * four inline rtps words are byte-identical; the sprite base remains a
 * compiler operand rather than a named register.
 *
 * Residual: six nop differences with three loads/four stores, plus one
 * inverted branch around always-executed scaling that retail guards with a
 * literal zero. The remaining work is source shape and scheduling.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../game/ordering_tables.h"

typedef struct {
    u8 b[8];
} Blk8;

typedef struct {
    void *model;
    u8 pad_04[0x14];
    s8 f18;
} Holder;

extern u8 D_800FE148[];

extern s32 func_800879A0(void *);

void func_80015EF4(Holder *holder, u8 *prim, u8 *sprite, GsOT *ot)
{
    u8 *m;
    s32 t;
    s32 c;
    s32 d;
    s32 y;
    SVECTOR *q;
    SVECTOR *rot;
    SVECTOR *cpy;
    VECTOR *scale;
    SVECTOR *up;
    MATRIX *lm;
    s32 *depth;

    m = (u8 *)holder->model;
    if ((*(u16 *)(m + 8) & 0x40) == 0) {
        return;
    }

    q = (SVECTOR *)0x1F800300;
    rot = (SVECTOR *)0x1F8003A0;
    cpy = (SVECTOR *)0x1F8003C0;
    scale = (VECTOR *)0x1F800320;
    up = (SVECTOR *)0x1F800380;
    lm = (MATRIX *)0x1F8002C0;
    depth = (s32 *)0x1F8003E0;
    up->vx = 0;
    up->vy = 0x1000;
    up->vz = 0;
    *(s32 *)ot = *(s32 *)(m + 0xC);
    rot[0].vx = m[0x20] << 4;
    y = m[0x21] << 4;
    rot[0].vy = y;
    rot[0].vz = m[0x22] << 4;
    if (holder->f18 >= 0xF) {
        rot[0].vy = (m[0x21] << 4) + 0x800;
    }
    lm->t[0] = *(s16 *)(m + 0x30);
    lm->t[1] = *(s16 *)(m + 0x32);
    lm->t[2] = *(s16 *)(m + 0x34);
    t = *(s16 *)(m + 0x32) + 0xFF;
    if (t < 0) {
        t = 0;
    }
    sprite[6] = t;
    sprite[5] = t;
    sprite[4] = t;
    if (RotMatrixZYX_gte(rot, lm) != 0) {
        scale->vx = *(s16 *)(m + 0x44);
        scale->vy = 0x1000;
        scale->vz = *(s16 *)(m + 0x46);
        ScaleMatrix(lm, scale);
    }

    q[2].vx = q[0].vx = -0x19;
    q[3].vx = q[1].vx = 0x1A;
    q[1].vz = q[0].vz = 0x1D;
    q[3].vz = q[2].vz = -0x1E;
    q[3].vy = q[2].vy = q[1].vy = q[0].vy = 0;
    GsSetLsMatrix(lm);

    RotTransSV(&q[0], &rot[0], (long *)depth);
    RotTransSV(&q[1], &rot[1], (long *)depth);
    RotTransSV(&q[2], &rot[2], (long *)depth);
    RotTransSV(&q[3], &rot[3], (long *)depth);

    *(Blk8 *)&cpy[0] = *(Blk8 *)&rot[0];
    *(Blk8 *)&cpy[1] = *(Blk8 *)&rot[1];
    *(Blk8 *)&cpy[2] = *(Blk8 *)&rot[2];
    *(Blk8 *)&cpy[3] = *(Blk8 *)&rot[3];

    GsSetLightMatrix(lm);
    GsSetLsMatrix((MATRIX *)D_800FE148);

    RotColorDpq(&rot[0], up, (CVECTOR *)ot,
                (long *)(prim + 8), (CVECTOR *)(prim + 4),
                (long *)&depth[0]);
    RotColorDpq(&rot[1], up, (CVECTOR *)ot,
                (long *)(prim + 0x14), (CVECTOR *)(prim + 0x10),
                (long *)&depth[1]);
    RotColorDpq(&rot[2], up, (CVECTOR *)ot,
                (long *)(prim + 0x20), (CVECTOR *)(prim + 0x1C),
                (long *)&depth[2]);
    RotColorDpq(&rot[3], up, (CVECTOR *)ot,
                (long *)(prim + 0x2C), (CVECTOR *)(prim + 0x28),
                (long *)&depth[3]);

    if ((depth[0] | depth[1] | depth[2]
         | depth[3]) < 0) {
        return;
    }

    c = m[0x5C];
    prim[0x24] = c;
    prim[0xC] = c;
    c = m[0x5D];
    prim[0x19] = c;
    prim[0xD] = c;
    if (func_800879A0(prim) <= 0) {
        prim[0x24] = 0x38;
        prim[0xC] = 0x38;
        prim[0x19] = 0x80;
        prim[0xD] = 0x80;
    }
    c = prim[0xC] + 0x34;
    d = prim[0xD] + 0x3C;
    prim[0x30] = c;
    prim[0x18] = c;
    prim[0x31] = d;
    prim[0x25] = d;
    prim[3] = 0xC;
    prim[7] = 0x3C;
    *(s16 *)(prim + 0xE) = ((*(u16 *)(m + 0x42) + 0xF1) << 6) | 0x10;
    SetSemiTrans(prim, 0);
    *(u16 *)(prim + 0x1A) = *(u16 *)(prim + 0x1A) & 0xFF9F;
    *(u16 *)(prim + 0x1A) = *(u16 *)(prim + 0x1A)
                            | ((*(u32 *)(m + 4) >> 23) & 0x60);
    if ((*(u32 *)(m + 4) & 0x40000000) != 0) {
        SetSemiTrans(prim, 1);
        sprite[4] = m[0xC];
        sprite[5] = m[0xD];
        sprite[6] = m[0xE];
    }

    depth[0] = (depth[0] + depth[1]
                          + depth[2] + depth[3]) / 16;
    GsSortPoly(prim, D_800E9D90[2], *(u16 *)depth);

    cpy[3].vy = 0;
    cpy[2].vy = 0;
    cpy[1].vy = 0;
    cpy[0].vy = 0;
    __asm__ volatile(
        "lui $8, 0x1F80\n"
        "ori $8, $8, 0x03C0\n"
        "lwc2 $0, 0($8)\n"
        "lwc2 $1, 4($8)\n"
        "nop\n"
        "nop\n"
        ".word 0x4A180001\n"
        "addiu $2, %0, 8\n"
        "swc2 $14, 0($2)\n"
        "lui $9, 0x1F80\n"
        "ori $9, $9, 0x03C8\n"
        "lwc2 $0, 0($9)\n"
        "lwc2 $1, 4($9)\n"
        "nop\n"
        "nop\n"
        ".word 0x4A180001\n"
        "addiu $2, %0, 16\n"
        "swc2 $14, 0($2)\n"
        "lui $10, 0x1F80\n"
        "ori $10, $10, 0x03D0\n"
        "lwc2 $0, 0($10)\n"
        "lwc2 $1, 4($10)\n"
        "nop\n"
        "nop\n"
        ".word 0x4A180001\n"
        "addiu $2, %0, 24\n"
        "swc2 $14, 0($2)\n"
        "lui $11, 0x1F80\n"
        "ori $11, $11, 0x03D8\n"
        "lwc2 $0, 0($11)\n"
        "lwc2 $1, 4($11)\n"
        "nop\n"
        "nop\n"
        ".word 0x4A180001\n"
        "addiu $2, %0, 32\n"
        "swc2 $14, 0($2)\n"
        : : "r"(sprite) : "$2", "$8", "$9", "$10", "$11", "memory");
    GsSortPoly(sprite, D_800E9D90[2], 0xFFF);
}
