#include "../../../../src/types.h"
#include "../../../../src/psyq/libgte.h"
#include "../../../../src/psyq/libgpu.h"
#include "../../../../src/psyq/inline_c.h"

extern void func_80029684(s32, s32, s16 *, s32 *, s32, s32);
extern void func_800297DC(s32, s32, s16 *, s32 *, s32, s32);
extern void func_8005B260(s32, s32, s32, s32);
extern void func_800540B4(s32);
extern void func_800559D4(s32);
extern void func_800556E8(s32);
extern void GsSetLsMatrix(MATRIX *);

extern s32 D_800E9D9C;
extern MATRIX D_800FE148;
extern s16 D_800EA1E8[];
extern u8 D_80181000[];

typedef struct {
    u8 b[8];
} Blk8;

void func_80029934(void)
{
    u8 *prim;
    s16 *vec;
    s16 *vec1;
    s16 *vec2;
    s16 *vec3;
    s32 *ctl;
    s16 *par;
    s32 angle;
    s32 arg;
    s32 t;
    s32 i;
    s32 step;
    s32 y;

    prim = (u8 *)0x1F800000;
    vec = (s16 *)0x1F800038;
    vec1 = (s16 *)0x1F800040;
    vec2 = (s16 *)0x1F800048;
    vec3 = (s16 *)0x1F800050;
    ctl = (s32 *)0x1F800060;
    par = (s16 *)0x1F800200;

    angle = 0;
    SetGeomOffset(0xD0, 0x60);
    SetGeomScreen(0x12C);
    arg = D_800E9D9C;
    GsSetLsMatrix(&D_800FE148);

    *(Blk8 *)par = *(Blk8 *)D_80181000;

    prim[3] = 7;
    *(s32 *)(prim + 4) = 0;
    prim[7] = 0x58;
    *(s32 *)(prim + 0x1C) = 0x55555555;
    prim[0x17] = 0;
    *(s16 *)0x1F800052 = 0;
    *(s16 *)0x1F80004A = 0;
    *(s16 *)0x1F800042 = 0;
    vec[1] = 0;

    do {
        t = (0x400 - angle) * 0xFF / 0x400;
        prim[0xE] = t;
        prim[0xD] = t;
        prim[0xC] = t;
        *(s32 *)(prim + 0x14) = *(s32 *)(prim + 0xC);
        { s16 *t1 = D_800EA1E8; ctl[2] = rcos(angle) * t1[8] / 4096; }
        if (ctl[2] == 0) {
            break;
        }
        { s16 *t1 = D_800EA1E8; ctl[3] = rsin(angle) * t1[8] / 4096; }
        vec3[2] = vec2[2] = vec1[2] = vec[2] = (u16)ctl[3] + (u16)par[2];
        func_80029684((s32)prim, arg, vec, ctl, par[0], ctl[2]);
        func_80029684((s32)prim, arg, vec, ctl, par[0], -ctl[2]);
        if (angle != 0) {
            vec3[2] = vec2[2] = vec1[2] = vec[2] = (u16)par[2] - (u16)ctl[3];
            func_80029684((s32)prim, arg, vec, ctl, par[0], ctl[2]);
            func_80029684((s32)prim, arg, vec, ctl, par[0], -ctl[2]);
        }
        vec[12] = vec[8] = vec[4] = vec[0] = (u16)par[0] + (u16)ctl[3];
        func_800297DC((s32)prim, arg, vec, ctl, par[2], ctl[2]);
        func_800297DC((s32)prim, arg, vec, ctl, par[2], -ctl[2]);
        if (angle != 0) {
            vec[12] = vec[8] = vec[4] = vec[0] = (u16)par[0] - (u16)ctl[3];
            func_800297DC((s32)prim, arg, vec, ctl, par[2], ctl[2]);
            func_800297DC((s32)prim, arg, vec, ctl, par[2], -ctl[2]);
        }
        angle += 0x80;
    } while (angle < 0x400);

    angle = 0;
    prim[3] = 7;
    *(s32 *)(prim + 4) = 0;
    *(s32 *)(prim + 0xC) = 0xFFFFFF;
    *(s32 *)(prim + 0x14) = 0;
    prim[7] = 0x58;
    *(s32 *)(prim + 0x1C) = 0x55555555;
    prim[0x17] = 0;
    vec[1] = 0;
    vec[5] = 0;
    do {
        step = D_800EA1E8[8] + 0x80;
        vec[0] = (u16)par[0] + step * rcos(angle) / 4096;
        vec[4] = (u16)par[0] + step * rcos(angle + 0x80) / 4096;
        vec[2] = (u16)par[2] + step * rsin(angle) / 4096;
        vec[6] = (u16)par[2] + step * rsin(angle + 0x80) / 4096;
        step = step / 10;
        __asm__ volatile(
            "lwc2 $0, 0($18)\n"
            "lwc2 $1, 4($18)\n"
            "nop\n"
            "nop\n"
            ".word 0x4A180001\n" /* rtps */
            "addiu $2, $19, 0x08\n"
            "swc2 $14, 0($2)\n"
            "addiu $2, $18, 0x8\n"
            "lwc2 $0, 0($2)\n"
            "lwc2 $1, 4($2)\n"
            "nop\n"
            "nop\n"
            ".word 0x4A180001\n" /* rtps */
            "addiu $2, $19, 0x18\n"
            "swc2 $14, 0($2)\n"
            : : : "$2", "memory");
        i = 0;
        y = *(u16 *)(prim + 0xA) - step;
        *(s16 *)(prim + 0x10) = *(u16 *)(prim + 8);
        *(s16 *)(prim + 0x1A) = y;
        *(s16 *)(prim + 0x12) = y;
        do {
            func_8005B260((s32)prim, arg, 1, 1);
            *(s16 *)(prim + 0xA) = *(u16 *)(prim + 0xA) - step;
            if ((s16)*(u16 *)(prim + 0xA) <= 0) {
                break;
            }
            i++;
            *(s16 *)(prim + 0x12) = *(u16 *)(prim + 0x12) - step;
            *(s16 *)(prim + 0x1A) = *(u16 *)(prim + 0x1A) - step;
        } while (i < 7);
        angle += 0x80;
    } while (angle < 0x1000);

    func_800540B4(0);
    func_800559D4(0);
    func_800556E8(0);
}
