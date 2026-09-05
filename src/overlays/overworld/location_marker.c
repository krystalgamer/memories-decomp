#include "../../types.h"

extern u8 gCampaignMap_aLocationTable[];
extern u8 gCampaignMap_Location;
extern u8 D_801AF000[];
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_800428A8(void *, s32, s32, s32, s32, s32, s32, s32, void *);
extern void func_800428EC(void *, s32);

u8 *func_80168588(s32 index)
{
    u8 *object;
    u8 *table;
    u8 *record;
    u8 *entry;

    object = func_800400AC(func_8004002C(), 2);
    table = gCampaignMap_aLocationTable;
    record = table + gCampaignMap_Location * 66;
    func_800428A8(object, *(s16 *)(record + 0xC), *(s16 *)(record + 0xE), 0, 1, 0,
                  0x17, 0x100, D_801AF000);
    func_800428EC(object, 0xA);
    *(u16 *)(object + 8) |= 0x28;
    entry = table + index * 66;
    *(u16 *)(object + 0x30) = *(u16 *)(entry + 0xC);
    *(u16 *)(object + 0x32) = *(u16 *)(entry + 0xE);
    return object;
}
