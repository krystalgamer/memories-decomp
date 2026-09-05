#include "../../types.h"

extern u8 D_800F2848[];
extern volatile u16 D_8009B3A4;
extern void func_8001352C(void);

void CampaignMap_MoveCameraDpad(void)
{
    u8 *camera = D_800F2848;
    s32 step;

    if ((D_8009B3A4 & 0xF00C) != 0) {
        if ((D_8009B3A4 & 0x3) != 0) {
            if ((D_8009B3A4 & 0xA000) != 0) {
                if ((D_8009B3A4 & 0x40) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((D_8009B3A4 & 0x8000) != 0) {
                    step = -step;
                }
                *(s32 *)(camera + 0x1C) = *(s32 *)(camera + 0x1C) + step;
            }
            if ((D_8009B3A4 & 0x5000) != 0) {
                if ((D_8009B3A4 & 0x40) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((D_8009B3A4 & 0x4000) != 0) {
                    step = -step;
                }
                *(s32 *)(camera + 0x24) = *(s32 *)(camera + 0x24) + step;
            }
        } else {
            if ((D_8009B3A4 & 0xA000) != 0) {
                if ((D_8009B3A4 & 0x40) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((D_8009B3A4 & 0x8000) != 0) {
                    step = -step;
                }
                *(u16 *)(camera + 2) = *(u16 *)(camera + 2) + step;
            }
            if ((D_8009B3A4 & 0x5000) != 0) {
                if ((D_8009B3A4 & 0x40) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((D_8009B3A4 & 0x4000) != 0) {
                    step = -step;
                }
                *(u16 *)(camera + 4) = *(u16 *)(camera + 4) + step;
            }
            if ((D_8009B3A4 & 0xC) != 0) {
                if ((D_8009B3A4 & 0x40) != 0) {
                    step = 20;
                } else {
                    step = 4;
                }
                if ((D_8009B3A4 & 0x8) != 0) {
                    step = -step;
                }
                *(u16 *)(camera + 0) = *(u16 *)(camera + 0) + step;
            }
        }
        func_8001352C();
    }
}
