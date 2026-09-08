#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "model.h"

typedef struct {
    u32 flg;
    MATRIX coord;
    MATRIX workm;
} Coord;

typedef struct {
    Coord co;
    SVECTOR zero;
    s32 pad5C;
} CoordLocals;

extern u16 D_8009B47A;
extern u16 D_8009B47C;

extern void func_8008A4A0(u8 *, MATRIX *);
extern void func_80088E50(SVECTOR *, MATRIX *);
extern MATRIX *func_800889C0(SVECTOR *, MATRIX *);
extern void *memset(void *, s32, s32);

void func_800580D4(s32 index, s32 arg1, u8 *arg2, u8 *arg3)
{
    CoordLocals locals;
    SVECTOR ang;
    MATRIX ls;
    SVECTOR sv88;
    SVECTOR sv90;
    MATRIX work;
    u8 scratch[8];
    u8 *p;
    s32 z;
    register s32 turn asm("$6");
    /* Prevent GCC from carrying &work across the final pair of calls. */
    register u8 *stack_pointer asm("$sp");

    p = (u8 *)D_800F2C40 + index * MODEL_SLOT_SIZE;
    if (p[0xE17] < arg1) {
        arg1 = p[0xE18];
    }

    func_8008A4A0(*(u8 **)(p + 0xD14) + arg1 * 0x50, &ls);
    GsSetLsMatrix(&ls);

    RotTransSV((SVECTOR *)arg2, &ang, (long *)scratch);

    locals.zero.vz = 0;
    locals.zero.vy = 0;
    locals.zero.vx = 0;
    func_80088E50(&locals.zero, &locals.co.coord);

    locals.co.flg = 1;
    locals.co.coord.t[0] = ang.vx;
    locals.co.coord.t[1] = ang.vy;
    locals.co.coord.t[2] = ang.vz;
    locals.co.workm = locals.co.coord;
    locals.pad5C = 0;
    memset(&sv90, 0, 8);

    turn = 0x1000;
    sv90.vy = turn - D_8009B47A;
    sv90.vz = D_8009B47C;
    sv88 = sv90;

    *(s32 *)arg3 = 0;
    *(s16 *)(arg3 + 0x44) = turn - *(u16 *)((*(u8 * volatile *)((u8 *)D_800F2C40 + 0xD18)) + 0x44);
    *(s16 *)(arg3 + 0x46) = turn - *(u16 *)((*(u8 * volatile *)((u8 *)D_800F2C40 + 0xD18)) + 0x46);
    z = turn - *(u16 *)((*(u8 * volatile *)((u8 *)D_800F2C40 + 0xD18)) + 0x48);
    *(s16 *)(arg3 + 0x48) = z;
    func_800889C0((SVECTOR *)(arg3 + 0x44), (MATRIX *)(arg3 + 4));

    func_80088E50(&sv88, &work);
    MulMatrix((MATRIX *)(arg3 + 4), (MATRIX *)(stack_pointer + 0x98));

    *(s32 *)(arg3 + 0x20) = 0;
    *(s32 *)(arg3 + 0x1C) = 0;
    *(s32 *)(arg3 + 0x18) = 0;
    *(Coord **)(arg3 + 0x4C) = &locals.co;
}
