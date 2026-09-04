#include "../../types.h"

extern u8 gCampaignMap_aLocationTable[];
extern u8 D_800F2848[];
extern void func_8001352C(void);

void func_801681E8(s32 index)
{
    u8 *entry = gCampaignMap_aLocationTable + index * 66;
    u8 *camera = D_800F2848;

    *(u16 *)(camera + 4) = *(u16 *)(entry + 2);
    *(u16 *)(camera + 2) = *(u16 *)(entry + 4);
    *(u16 *)(camera + 0) = *(u16 *)(entry + 6);
    *(s32 *)(camera + 0x1C) = *(s16 *)(entry + 8);
    *(s32 *)(camera + 0x24) = *(s16 *)(entry + 0xA);
    func_8001352C();
}
