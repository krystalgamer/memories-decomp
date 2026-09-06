#include "../../types.h"

extern u8 gCampaignMap_aLocationTable[];
extern u8 D_801AF000[];
extern u8 *D_801695F8[];
extern void CampaignMap_ClearLocationObjects(void);
extern s32 Campaign_TestStoryFlag(s32);
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_800428A8(void *, s32, s32, s32, s32, s32, s32, s32, void *);
extern void func_800428EC(void *, s32);

void CampaignMap_RebuildLocationObjects(s32 index)
{
    u8 *record;
    u8 *entry;
    u8 *object;
    s32 i;
    s32 offset;

    CampaignMap_ClearLocationObjects();
    record = gCampaignMap_aLocationTable + index * 66;
    for (i = 0; i < 4; i++) {
        offset = i * 12 + 0x12;
        entry = record + offset;
        if (entry[9] != 0x10) {
            if (*(u16 *)entry == 0 ||
                Campaign_TestStoryFlag(*(u16 *)entry) != 0) {
                object = func_800400AC(func_8004002C(), 2);
                func_800428A8(object, *(s16 *)(entry + 2), *(s16 *)(entry + 4), 0, 2,
                              entry[8], 0x17, 0x100, D_801AF000);
                func_800428EC(object, 5);
                *(u16 *)(object + 8) |= 0x28;
                D_801695F8[i] = object;
            }
        }
    }
}
