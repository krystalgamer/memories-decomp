#include "../types.h"

typedef void (*ObjFn)(u8 *);

extern s32 D_8009B350;
extern s8 D_8009B34D;
extern s8 D_8009B345;
extern u8 D_8009B34C;
extern u8 D_8009B336;
extern u8 D_8009B327;
extern ObjFn D_8009B340;

extern void Text_SetCursorOffset(u8 *);
extern void func_80037CE0(u8 *);
extern void func_80035CA8(s32);
extern void DuelEffect_ClearMatchingMarker(s32);

void Text_HandleChoiceCommand(u8 *p)
{
    s32 t;
    s32 u;
    s32 w;
    s32 v;
    s32 c;
    s32 d;

    D_8009B350 = 1;
    t = *(*(u8 **)(p - -(*(s8 *)(p + 0x58) * 4)))++;
    c = t;
    d = 0xF;
    if (c & 8) {
        u = *(*(u8 **)(p - -(*(s8 *)(p + 0x58) * 4)))++;
        t = u;
        d = t;
    }
    if (c & 0x80) {
        *(s32 *)(p - -(*(s8 *)(p + 0x58) * 4)) += D_8009B34D * 2;
        Text_SetCursorOffset(p);
    } else {
        D_8009B345 = 7;
        D_8009B345 = c & D_8009B345;
        D_8009B34C = c & 0xF0;
        D_8009B336 = d & 0xF;
        w = d & 0x80;
        D_8009B34D = 0;
        D_8009B327 = 0;
        if (w != 0) {
            D_8009B327 = 1;
        }
        func_80035CA8(p[0x57]);
        DuelEffect_ClearMatchingMarker(p[0x57]);
        v = *(u16 *)(p + 0x34);
        p[0x56] = 0;
        D_8009B340 = func_80037CE0;
        *(u16 *)(p + 0x34) = v | 0x1000;
    }
}
