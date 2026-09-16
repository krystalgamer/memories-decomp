/*
 * Renders the 3D duel card through scratchpad matrices, four projected
 * corners, per-vertex lighting, and two ordering-table submissions. Current
 * best under gcc_2_8_1_g0: 385/386 instructions and opcode multiset distance
 * 7 (GTE commands the census renders as c2 left out), with no hard register
 * pin.
 *
 * Scratchpad pointers are initialized after the early guard, typed bases
 * preserve the retail address mix, and Bytes8 assignments produce the
 * unaligned corner copies. The four RotColorDpq returns are kept in
 * depth[4..7] and averaged the way retail does, as (sum / 4) >> 2; NormalClip
 * takes the three projected words; the raised Y rotation reuses y; the Y
 * translation is read once into c; and both GsSortPoly calls read the
 * ordering-table array through tab. The four inline rtps words are
 * byte-identical; the sprite base remains a compiler operand rather than a
 * named register.
 *
 * Residual: four instructions, with two axes measured and closed. Discount the
 * census pair c2 +4 / rtps -4 throughout: those are the same four GTE words
 * spelled differently by the two renderers.
 *
 * The two extra nops are one phenomenon rather than two. Retail splits an
 * address's %hi/%lo pair and puts half of it in a delay slot -- addiu %lo of
 * D_800FE148 in the jal GsSetLsMatrix slot, lui %hi of D_800E9D90 in the bgez
 * slot -- while this source keeps each pair adjacent and the slot takes a nop.
 * The declarations are not the axis: MATRIX is 32 bytes and the GsOT * array
 * 16, so both symbols are already outside small data, and the target listing
 * has no %gp_rel and no lui $at at all. The profile is: under
 * gcc_2_8_1_g0_split both nops go (surplus 3 -> 1, non-equal blocks 22 -> 18),
 * and the length then reads 383/386 instead of 385/386 -- the 385 was two
 * faults cancelling, not agreement. Control: gcc_2_8_1_g0_no_split is
 * byte-identical to the installed profile, which is what shows the default is
 * already the unsplit form. Not installed here, because the deficit below is
 * still open and the change would be argued on the census alone.
 *
 * The inverted branch is a dead branch that retail emits anyway: it zeroes $v0
 * after the RotMatrixZYX_gte call and branches bnez on that zero, so the
 * scaling always runs and the call's return value is discarded. A local zero
 * tested from another basic block does not reproduce it -- gcc folds the test
 * and deletes the branch, measured at two positions, both identical to each
 * other. Deleting the branch outright is worse, not better: it trades the
 * surplus beqz for a missing bnez and addu.
 *
 * The last addu/ori is retail holding 0x1F800000 intact in a register and
 * deriving rot by copy plus or, where this source spends a fresh lui/ori.
 * Respelling the base does not reach it -- a base local added, or-ed, or used
 * for rot alone all compile byte-identically to the plain constants, because
 * the base is a compile-time constant and gcc folds every form. The sharing is
 * the compiler's own CSE decision, so what is left to try is position and
 * register pressure, not further spellings of the constant.
 */
#include "../types.h"
#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../game/screen_projection.h"
#include "../game/ordering_tables.h"

void func_80015EF4(
    DuelCardRenderHolder *holder,
    u8 *prim,
    u8 *sprite,
    GsOT *ot
)
{
    GsOT **tab;
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
    if (holder->field_18 >= 0xF) {
        rot[0].vy = y + 0x800;
    }
    lm->t[0] = *(s16 *)(m + 0x30);
    c = *(s16 *)(m + 0x32);
    lm->t[1] = c;
    lm->t[2] = *(s16 *)(m + 0x34);
    t = c + 0xFF;
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

    *(Bytes8 *)&cpy[0] = *(Bytes8 *)&rot[0];
    *(Bytes8 *)&cpy[1] = *(Bytes8 *)&rot[1];
    *(Bytes8 *)&cpy[2] = *(Bytes8 *)&rot[2];
    *(Bytes8 *)&cpy[3] = *(Bytes8 *)&rot[3];

    GsSetLightMatrix(lm);
    GsSetLsMatrix(&D_800FE148);

    depth[4] = RotColorDpq(&rot[0], up, (CVECTOR *)ot,
                (long *)(prim + 8), (CVECTOR *)(prim + 4),
                (long *)&depth[0]);
    depth[5] = RotColorDpq(&rot[1], up, (CVECTOR *)ot,
                (long *)(prim + 0x14), (CVECTOR *)(prim + 0x10),
                (long *)&depth[1]);
    depth[6] = RotColorDpq(&rot[2], up, (CVECTOR *)ot,
                (long *)(prim + 0x20), (CVECTOR *)(prim + 0x1C),
                (long *)&depth[2]);
    depth[7] = RotColorDpq(&rot[3], up, (CVECTOR *)ot,
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
    if (NormalClip(*(long *)(prim + 8), *(long *)(prim + 0x14), *(long *)(prim + 0x20)) <= 0) {
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

    depth[0] = (depth[4] + depth[5]
                          + depth[6] + depth[7]) / 4 >> 2;
    tab = D_800E9D90;
    GsSortPoly(prim, tab[2], *(u16 *)depth);

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
    GsSortPoly(sprite, tab[2], 0xFFF);
}
