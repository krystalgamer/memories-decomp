#define GINPUT_PAD1_HELD_IS_VOLATILE
#include "../../types.h"
#include "../../unmatched.h"
#include "camera_state.h"
#include "location_objects.h"
#include "location_marker.h"
#include "../../game/campaign_flags.h"
#include "../../game/model_slot_setup.h"
#include "../../game/model_scene_setup.h"
#include "../../game/display_object_api.h"
#include "../../psyq/libgte.h"
#include "../../game/sound.h"
#include "../../game/display_object_helpers.h"
#include "../../game/text_box_runtime.h"
#include "../../game/input.h"
#include "../../game/sorted_entry.h"
#include "campaign_map.h"
#include "../../game/view_state.h"
#include "../../game/main_services.h"

extern u8 D_80169619;
extern u8 *D_801695F8_objects[] asm("D_801695F8");
extern s32 D_801695F8_words[] asm("D_801695F8");
extern s32 D_80010000;
extern void func_800530C4(void);
extern u8 *func_80058F74(s32);
extern void func_8005922C(u8 *, s32 *);
extern void func_80035668(s32);
extern void func_800857C0(int);
extern void *func_80035BE4(s32, s32, s32, s32, s32, s32);

void CampaignMap_ClearLocationObjects(void)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        func_8004036C(D_801695F8_objects[i]);
        D_801695F8_objects[i] = 0;
    }
}

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
                func_800428A8(
                    object, *(s16 *)(entry + 2), *(s16 *)(entry + 4), 0, 2,
                    entry[8], 0x17, 0x100, D_801AF000
                );
                func_800428EC(object, 5);
                *(u16 *)(object + 8) |= 0x28;
                D_801695F8_objects[i] = object;
            }
        }
    }
}

u8 *CampaignMap_CreateLocationLabel(s32 unused)
{
    u8 *object;

    object = func_80035BE4(
        0, gCampaignMap_Location + 0x8350, 0x60, 0x18, 0x80, 0xC
    );
    func_80039A60(object);
    return object;
}

void CampaignMap_SetCameraFromLocation(s32 index)
{
    u8 *entry = gCampaignMap_aLocationTable + index * 66;
    ViewState *camera = &D_800F2848;

    camera->field_04 = *(u16 *)(entry + 2);
    camera->angle = *(u16 *)(entry + 4);
    camera->field_00 = *(u16 *)(entry + 6);
    camera->view.vrx = *(s16 *)(entry + 8);
    camera->view.vrz = *(s16 *)(entry + 0xA);
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
    u8 *matrix = (u8 *)&D_800F2848 + 0x10;

    camera->field_00 = 0x6A4;
    camera->angle = 0x640;
    camera->field_04 = 0x180;
    camera->field_0C = 0;
    camera->view.rz = 0;
    camera->view.super = 0;
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
                camera->view.vrx = camera->view.vrx + step;
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
                camera->view.vrz = camera->view.vrz + step;
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

u8 *CampaignMap_CreateLocationMarker(s32 index)
{
    u8 *object;
    u8 *table;
    u8 *record;
    u8 *entry;

    object = func_800400AC(func_8004002C(), 2);
    table = gCampaignMap_aLocationTable;
    record = table + gCampaignMap_Location * 66;
    func_800428A8(
        object, *(s16 *)(record + 0xC), *(s16 *)(record + 0xE), 0, 1, 0,
        0x17, 0x100, D_801AF000
    );
    func_800428EC(object, 0xA);
    *(u16 *)(object + 8) |= 0x28;
    entry = table + index * 66;
    *(u16 *)(object + 0x30) = *(u16 *)(entry + 0xC);
    *(u16 *)(object + 0x32) = *(u16 *)(entry + 0xE);
    return object;
}

void CampaignMap_SetLocation(s32 index)
{
    u8 *obj;
    s32 track;
    s32 i;
    s32 location;
    u8 *marker;
    u8 *panel;
    u16 flags;
    s32 colour[3];

    gCampaignMap_Location = index;
    D_80169619 = 0;
    D_8016960D = 0;
    D_801695EC = 0;
    D_801695C8 = 0;
    for (i = 3; i >= 0; i--) {
        D_801695F8_words[i] = 0;
    }
    func_800530C4();
    func_800533D8();
    func_80056250(2, (u8 *)D_80010000, 0x43000, 0);
    obj = func_80058F74(2);
    colour[0] = 1365;
    colour[1] = 1365;
    colour[2] = 1365;
    func_8005922C(obj, colour);
    CampaignMap_ResetCamera();
    SetFarColor(0, 0, 0);
    SetFogNearFar(6000, 8000, D_800F2848.projection);
    D_800E9DB0[3] = CampaignMap_UpdateView;
    func_80035668(0);
    obj = func_800400AC(func_8004002C(), 2);
    func_800428A8(obj, 96, 24, 0, 0, 0, 23, 256, D_801AF000);
    *(u16 *)(obj + 8) = *(u16 *)(obj + 8) | 0x28;
    obj = func_800400AC(func_8004002C(), 6);
    *(s16 *)(obj + 0x30) = 160;
    *(s16 *)(obj + 0x32) = 144;
    *(s16 *)(obj + 0x60) = 128;
    *(s16 *)(obj + 0x48) = 32;
    *(s16 *)(obj + 0x4A) = 192;
    *(s16 *)(obj + 0x44) = 5120;
    *(s16 *)(obj + 0x46) = 4096;
    func_800428EC(obj, -10);
    location = gCampaignMap_Location;
    *(void **)(obj + 0x4C) = func_80042C08;
    D_801695D8 = (MapObject *)obj;
    CampaignMap_SetCameraFromLocation(location);
    CampaignMap_CreateLocationLabel(gCampaignMap_Location);
    CampaignMap_RebuildLocationObjects(gCampaignMap_Location);
    gCampaignMap_LocationPrev = gCampaignMap_Location;
    if ((u8)gCampaignMap_Location >= 10) {
        marker = CampaignMap_CreateLocationMarker(gCampaignMap_Location);
        panel = (u8 *)D_801695D8;
        flags = *(u16 *)(panel + 8);
        D_801695C8 = (MapObject *)marker;
        *(u16 *)(panel + 8) = flags & 0xFFBF;
    }
    track = 0x70A0;
    if (Campaign_TestStoryFlag(CAMPAIGN_FLAG_TOURNAMENT_COMPLETE) != 0) {
        track = 0x70B0;
    }
    SD_BGMPlay(track);
}
