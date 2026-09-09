#include "../../types.h"
#include "../../unmatched.h"
#include "camera_state.h"
#include "../../psyq/libgte.h"
#include "../../game/view_state.h"
#include "campaign_map.h"

extern u32 D_8009B304;
extern u32 D_8009B308;
extern u32 D_8009B30C;
extern u32 D_8009B310;
extern u32 D_8009B314;
extern volatile u16 gInput_wPad1Held;
extern void func_800540B4(int);
extern void func_800857C0(int);

void CampaignMap_SetCameraFromLocation(s32 index)
{
    u8 *entry = gCampaignMap_aLocationTable + index * 66;
    ViewState *camera = &D_800F2848;

    camera->field_04 = *(u16 *)(entry + 2);
    camera->angle = *(u16 *)(entry + 4);
    camera->field_00 = *(u16 *)(entry + 6);
    camera->field_1C = *(s16 *)(entry + 8);
    camera->field_24 = *(s16 *)(entry + 0xA);
    func_8001352C();
}

void CampaignMap_UpdateView(void)
{
    ViewState *camera = &D_800F2848;
    u32 flags;

    SetGeomScreen(camera->projection);
    SetGeomOffset(0xA0, 0x78);
    SetFarColor(0, 0, 0);
    SetFogNearFar(0x7D0, 0x960, camera->projection);

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
    ViewState *camera = &D_800F2848;
    /* A second base register, +0x10 into the same record, which is what
       retail uses to reach field_1C, field_20 and field_24 while the first
       one is still holding the halfword fields. */
    u8 *matrix = (u8 *)&D_800F2848 + 0x10;

    camera->field_00 = 0x6A4;
    camera->angle = 0x640;
    camera->field_04 = 0x180;
    camera->field_0C = 0;
    camera->field_28 = 0;
    camera->field_2C = 0;
    camera->projection = 0x12C;
    func_800857C0(0x12C);
    *(s32 *)(matrix + 0x0C) = 0;
    camera->field_06 = 0;
    *(s32 *)(matrix + 0x10) = 0;
    camera->field_08 = 0;
    *(s32 *)(matrix + 0x14) = 0;
    camera->field_0A = 0;
    func_8001352C();
}

void CampaignMap_MoveCameraDpad(void)
{
    ViewState *camera = &D_800F2848;
    s32 step;

    if ((gInput_wPad1Held & 0xF00C) != 0) {
        if ((gInput_wPad1Held & 0x3) != 0) {
            if ((gInput_wPad1Held & 0xA000) != 0) {
                if ((gInput_wPad1Held & 0x40) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((gInput_wPad1Held & 0x8000) != 0) {
                    step = -step;
                }
                camera->field_1C = camera->field_1C + step;
            }
            if ((gInput_wPad1Held & 0x5000) != 0) {
                if ((gInput_wPad1Held & 0x40) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((gInput_wPad1Held & 0x4000) != 0) {
                    step = -step;
                }
                camera->field_24 = camera->field_24 + step;
            }
        } else {
            if ((gInput_wPad1Held & 0xA000) != 0) {
                if ((gInput_wPad1Held & 0x40) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((gInput_wPad1Held & 0x8000) != 0) {
                    step = -step;
                }
                camera->angle = camera->angle + step;
            }
            if ((gInput_wPad1Held & 0x5000) != 0) {
                if ((gInput_wPad1Held & 0x40) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((gInput_wPad1Held & 0x4000) != 0) {
                    step = -step;
                }
                camera->field_04 = camera->field_04 + step;
            }
            if ((gInput_wPad1Held & 0xC) != 0) {
                if ((gInput_wPad1Held & 0x40) != 0) {
                    step = 20;
                } else {
                    step = 4;
                }
                if ((gInput_wPad1Held & 0x8) != 0) {
                    step = -step;
                }
                camera->field_00 = camera->field_00 + step;
            }
        }
        func_8001352C();
    }
}
