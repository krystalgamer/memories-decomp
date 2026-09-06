#include "../../types.h"

extern u8 D_800F2848[];
extern void func_800857C0(int);
extern void func_8001352C(void);

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
