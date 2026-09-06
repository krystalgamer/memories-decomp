#include "../types.h"

extern s32 gDuel_adwCardStats[];
extern u8 D_801A7AD8[];

u8 *func_8004002C(void);
u8 *func_800400AC(u8 *arg0, s32 arg1);
void func_80016778();
void func_80016D04();
void func_80042918(u8 *arg0);
void func_80017E3C(u8 *arg0);
void func_80017DB4(u8 *arg0);

/* Creates the sprite object for the duel card record at arg0 and fills its
 * five bytes at +0x67..+0x6B: the attribute from the card table entry
 * (index minus one), the record index as a pointer difference over 28, and
 * the owner byte through the record's +4 pointer. The caller func_80018004
 * passes only the record; arg1/arg2 are the +0x30/+0x32 position and retail
 * sets neither register there, so this file alone carries the full
 * prototype. */
u8 *func_80017F04(u8 *arg0, s32 arg1, s32 arg2) {
    u8 *p = func_800400AC(func_8004002C(), 6);

    s32 *tbl;
    s32 k;

    k = *(s16 *)(arg0 + 0xC) - 1;
    tbl = gDuel_adwCardStats;
    p[0x67] = 0;
    p[0x68] = (tbl[k] >> 26) & 0x1F;
    p[0x69] = 0;
    p[0x6A] = ((u32)arg0 - (u32)D_801A7AD8) / 28;
    p[0x6B] = (*(u8 **)(arg0 + 4))[2];
    *(s16 *)(p + 0x30) = arg1;
    *(s16 *)(p + 0x32) = arg2;
    *(s32 *)(p + 4) |= 0x1000000;
    *(void **)(p + 0x10) = func_80016778;
    func_80042918(p);
    *(void **)(p + 0x4C) = func_80016D04;
    func_80017E3C(p);
    func_80017DB4(p);
    return p;
}
