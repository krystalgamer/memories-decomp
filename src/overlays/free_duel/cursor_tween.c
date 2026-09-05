#include "../../types.h"

extern u8 *gFreeDuel_pCursorWidget;
extern u8 gFreeDuel_bScreenFlags;
extern s8 D_8009B366;
extern s8 D_8009B367;
extern s8 D_8009B36C;
extern s8 D_8009B36D;

extern void FreeDuel_PlaceCursor(void *, s32);
extern u8 **FreeDuel_GetSparkleSlot(void);
extern u8 *FreeDuel_SpawnSparkle(void);
extern void func_800429D8(void *);
extern void func_80042A78(void *);
extern void func_800428EC(void *, s32);
extern void func_80041D60(void *);
extern void func_8003FEE0(s32);

void FreeDuel_UpdateCursorTween(void)
{
    u8 *widget = gFreeDuel_pCursorWidget;
    u8 **slot;
    u8 *sparkle;
    s32 tx;
    s32 ty;
    s32 sx;
    s32 d;
    s16 left;

    if ((gFreeDuel_bScreenFlags & 0x40) == 0) {
        if (D_8009B366 == D_8009B36C && D_8009B367 == D_8009B36D) {
            return;
        }
        gFreeDuel_bScreenFlags |= 0x40;
        *(u16 *)(widget + 0x60) = 8;
        func_800429D8(widget);

        d = D_8009B36C;
        tx = d * 56 + 20;
        d = *(s16 *)(widget + 0x30);
        d = tx - d;
        sx = (d << 8) / 8;
        d = D_8009B36D;
        ty = d * 52 + 40;
        *(s16 *)(widget + 0x36) = sx;
        d = *(s16 *)(widget + 0x32);
        d = ty - d;
        *(s16 *)(widget + 0x38) = (d << 8) / 8;
    }

    func_80042A78(widget);
    left = *(u16 *)(widget + 0x60) - 1;
    *(u16 *)(widget + 0x60) = left;
    if (left == 0) {
        D_8009B366 = D_8009B36C;
        D_8009B367 = D_8009B36D;
        FreeDuel_PlaceCursor(widget, 1);
        gFreeDuel_bScreenFlags &= ~0x40;
        func_8003FEE0(47);
    } else {
        slot = FreeDuel_GetSparkleSlot();
        sparkle = FreeDuel_SpawnSparkle();
        if (sparkle != 0 && slot != 0) {
            *(u32 *)(sparkle + 0x30) = *(u32 *)(widget + 0x30);
            func_800428EC(sparkle, (s8)(widget[0x16] - 1));
            func_80041D60(sparkle);
            *(u32 *)(sparkle + 0x4C) = *(u32 *)(widget + 0x4C);
            sparkle[0x6C] = 1;
            *(u16 *)(sparkle + 8) |= 1;
            *slot = sparkle;
        }
    }
}
