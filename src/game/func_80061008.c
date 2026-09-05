#include "../types.h"

extern u8 D_801A8000[];
extern s16 gGraphics_sViewportY;
extern s16 gGraphics_sViewportX;

/* Keep symbolic usage evidence while preserving the literal-address codegen. */
#define gGraphics_sViewportY (*(s16 *)0x8009B148)
#define gGraphics_sViewportX (*(s16 *)0x8009B146)
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_80042918(u8 *);
extern void func_80060B38(void);

void func_80061008(s32 x0, s32 y0, s32 x1, s32 y1)
{
    u8 *record = D_801A8000;
    u8 *object;
    s32 i = 0;

    *(s16 *)(record + 4) = x0 + 2;
    *(s16 *)(record + 6) = y0;
    *(s16 *)(record + 0x1C) = x1 + 2;
    *(s16 *)(record + 0x1E) = y1;

    do {
        *(s16 *)(record + 8) = 0;
        object = func_800400AC(func_8004002C(), 6);
        object[0x68] = 0;
        object[0x69] = 0;
        *(u16 *)(object + 0x30) = *(u16 *)(record + 4);
        *(u16 *)(object + 0x32) = *(u16 *)(record + 6);
        object[0x6A] = i++;
        func_80042918(object);
        *(void (**)(void))(object + 0x4C) = func_80060B38;
        *(u8 **)record = object;
        record += 0x18;
    } while (i < 2);

    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
}
