#include "../../types.h"
#include "../../psyq/libgte.h"

extern u8 gCampaignMap_aLocationTable[];
extern u8 D_800F2848[];
extern u32 D_8009B304;
extern u32 D_8009B308;
extern u32 D_8009B30C;
extern u32 D_8009B310;
extern u32 D_8009B314;
extern void func_8001352C(void);
extern void func_800540B4(int);
extern void func_800857C0(int);

void CampaignMap_SetCameraFromLocation(s32 index)
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

void CampaignMap_UpdateView(void)
{
    u8 *camera = D_800F2848;
    u32 flags;

    SetGeomScreen(*(s16 *)(camera + 0xE));
    SetGeomOffset(0xA0, 0x78);
    SetFarColor(0, 0, 0);
    SetFogNearFar(0x7D0, 0x960, *(s16 *)(camera + 0xE));

    flags = D_8009B30C;
    if (flags & 2) {
        D_8009B314 += 0xA;
        if (D_8009B314 >= D_8009B308) {
            D_8009B30C = flags & ~3;
        }
        D_8009B310 = D_8009B304;
    }
    func_800540B4(2);
}

void CampaignMap_ResetCamera(void)
{
    u8 *camera = D_800F2848;
    u8 *matrix = D_800F2848 + 0x10;

    *(s16 *)(camera + 0x00) = 0x6A4;
    *(s16 *)(camera + 0x02) = 0x640;
    *(s16 *)(camera + 0x04) = 0x180;
    *(s16 *)(camera + 0x0C) = 0;
    *(s32 *)(camera + 0x28) = 0;
    *(s32 *)(camera + 0x2C) = 0;
    *(s16 *)(camera + 0x0E) = 0x12C;
    func_800857C0(0x12C);
    *(s32 *)(matrix + 0x0C) = 0;
    *(s16 *)(camera + 0x06) = 0;
    *(s32 *)(matrix + 0x10) = 0;
    *(s16 *)(camera + 0x08) = 0;
    *(s32 *)(matrix + 0x14) = 0;
    *(s16 *)(camera + 0x0A) = 0;
    func_8001352C();
}
