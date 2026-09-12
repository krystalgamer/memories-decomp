#define GINPUT_PAD1_HELD_IS_VOLATILE
#include "../../types.h"
#include "../../unmatched.h"
#include "camera_state.h"
#include "camera_transition.h"
#include "location_objects.h"
#include "location_marker.h"
#include "../../game/campaign_flags.h"
#include "../../game/model_slot_setup.h"
#include "../../game/model_scene_setup.h"
#include "../../game/display_object_api.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/sound.h"
#include "../../game/display_object_helpers.h"
#include "../../game/text_box_runtime.h"
#include "../../game/input.h"
#include "../../game/sorted_entry.h"
#include "../../game/trig_constants.h"
#include "../../game/display_object_interpolation.h"
#include "../../game/fade.h"
#include "campaign_map.h"
#include "../../game/view_state.h"
#include "../../game/main_services.h"
#include "../../game/model_slot_data.h"
#include "../../game/func_8005922C.h"
#include "../../game/text_render_state.h"
#include "../../game/text_box_lifecycle.h"

#include "../../game/high_memory_addresses.h"
extern u8 D_800E9ECE;
extern u8 D_800E9ECF;
extern u8 D_8009B26C;
extern u8 D_8009B27A;

void CampaignMap_ClearLocationObjects(void)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        func_8004036C(D_801695F8[i]);
        D_801695F8[i] = 0;
    }
}

void CampaignMap_RebuildLocationObjects(s32 index)
{
    MapLocation *record;
    CampaignMapExit *entry;
    u8 *object;
    s32 i;

    CampaignMap_ClearLocationObjects();
    record = gCampaignMap_aLocationTable + index;
    for (i = 0; i < 4; i++) {
        entry = &record->exits[i];
        if (entry->destination != 0x10) {
            if (entry->story_flag == 0 ||
                Campaign_TestStoryFlag(entry->story_flag) != 0) {
                object = func_800400AC(func_8004002C(), 2);
                func_800428A8(
                    object, entry->x, entry->y, 0, 2,
                    entry->field_08, 0x17, 0x100, D_801AF000
                );
                func_800428EC(object, 5);
                *(u16 *)(object + 8) |= 0x28;
                D_801695F8[i] = object;
            }
        }
    }
}

u8 *CampaignMap_CreateLocationLabel(s32 unused)
{
    u8 *object;

    object = TextBox_Create(
        0, gCampaignMap_Location + 0x8350, 0x60, 0x18, 0x80, 0xC
    );
    func_80039A60((struct DuelEffectChannel *)object);
    return object;
}

void CampaignMap_SetCameraFromLocation(s32 index)
{
    MapLocation *entry = gCampaignMap_aLocationTable + index;
    ViewState *camera = &D_800F2848;

    camera->field_04 = (u16)entry->camera_field_04;
    camera->angle = (u16)entry->camera_angle;
    camera->field_00 = (u16)entry->camera_field_00;
    camera->view.vrx = entry->view_x;
    camera->view.vrz = entry->view_z;
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
    GsSetProjection(0x12C);
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

    if ((gInput_wPad1Held &
         (PAD_DIRECTION_MASK | PAD_BUTTON_L1_R1_MASK)) != 0) {
        if ((gInput_wPad1Held & PAD_BUTTON_TRIGGER_MASK) != 0) {
            if ((gInput_wPad1Held & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
                if ((gInput_wPad1Held & PAD_BUTTON_CROSS) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((gInput_wPad1Held & PAD_DIRECTION_LEFT) != 0) {
                    step = -step;
                }
                camera->view.vrx = camera->view.vrx + step;
            }
            if ((gInput_wPad1Held & PAD_DIRECTION_VERTICAL_MASK) != 0) {
                if ((gInput_wPad1Held & PAD_BUTTON_CROSS) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((gInput_wPad1Held & PAD_DIRECTION_DOWN) != 0) {
                    step = -step;
                }
                camera->view.vrz = camera->view.vrz + step;
            }
        } else {
            if ((gInput_wPad1Held & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
                if ((gInput_wPad1Held & PAD_BUTTON_CROSS) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((gInput_wPad1Held & PAD_DIRECTION_LEFT) != 0) {
                    step = -step;
                }
                camera->angle = camera->angle + step;
            }
            if ((gInput_wPad1Held & PAD_DIRECTION_VERTICAL_MASK) != 0) {
                if ((gInput_wPad1Held & PAD_BUTTON_CROSS) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((gInput_wPad1Held & PAD_DIRECTION_DOWN) != 0) {
                    step = -step;
                }
                camera->field_04 = camera->field_04 + step;
            }
            if ((gInput_wPad1Held & PAD_BUTTON_L1_R1_MASK) != 0) {
                if ((gInput_wPad1Held & PAD_BUTTON_CROSS) != 0) {
                    step = 20;
                } else {
                    step = 4;
                }
                if ((gInput_wPad1Held & PAD_BUTTON_R1) != 0) {
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
    MapLocation *table;
    MapLocation *record;
    MapLocation *entry;

    object = func_800400AC(func_8004002C(), 2);
    table = gCampaignMap_aLocationTable;
    record = table + gCampaignMap_Location;
    func_800428A8(
        object, record->f12, record->f14, 0, 1, 0,
        0x17, 0x100, D_801AF000
    );
    func_800428EC(object, 0xA);
    *(u16 *)(object + 8) |= 0x28;
    entry = table + index;
    *(u16 *)(object + 0x30) = (u16)entry->f12;
    *(u16 *)(object + 0x32) = (u16)entry->f14;
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
        D_801695F8[i] = 0;
    }
    func_800530C4();
    func_800533D8();
    func_80056250(2, (u8 *)D_80010000, 0x43000, 0);
    obj = (u8 *)func_80058F74(2);
    colour[0] = 1365;
    colour[1] = 1365;
    colour[2] = 1365;
    func_8005922C((struct _GsCOORDUNIT *)obj, colour);
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

void CampaignMap_StartCameraTween(s32 index, s32 steps)
{
    ViewState *camera = &D_800F2848;
    s32 *cameraLong = (s32 *)&D_800F2848;
    MapLocation *entry = gCampaignMap_aLocationTable + index;
    s32 x;
    s32 y;
    s32 angle;
    s32 pitch;
    s32 dist;
    s32 stepX;
    s32 stepY;
    s32 turn;
    s32 stepTurn;
    s32 stepPitch;
    s32 stepDist;

    x = camera->field_00;
    stepX = ((entry->camera_field_00 - x) << 16) / steps;
    y = camera->field_04;
    stepY = ((entry->camera_field_04 - y) << 16) / steps;
    angle = camera->angle;
    turn = (entry->camera_angle - angle) & TRIG_ANGLE_MASK;
    D_801695E4 = (angle << 16) | 0x8000;
    D_801695E8 = (y << 16) | 0x8000;
    D_80169610 = (x << 16) | 0x8000;
    pitch = cameraLong[7];
    dist = cameraLong[9];
    D_801695CC = (pitch << 16) | 0x8000;
    D_801695D0 = (dist << 16) | 0x8000;
    D_80169614 = stepX;
    D_801695F4 = stepY;
    if (turn >= TRIG_ANGLE_HALF_TURN) {
        turn -= TRIG_ANGLE_FULL_TURN - 1;
    }
    stepTurn = (turn << 16) / steps;
    stepPitch = ((entry->view_x - pitch) << 16) / steps;
    stepDist = ((entry->view_z - dist) << 16) / steps;
    D_801695F0 = stepTurn;
    D_801695DC = stepPitch;
    D_801695E0 = stepDist;
}

s32 CampaignMap_UpdateLocationTransition(void)
{
    MapObject *obj;
    MapObject *marker;
    ViewState *cam;
    u16 flags;
    u16 raise;
    s32 step;
    s32 timer;
    s32 quotient;
    cam = &D_800F2848;
    cam = &D_800F2848;
    flags = D_801695EC;
    if ((flags & 0x80) == 0) {
        marker = D_801695C8;
        D_801695EC = flags | 0x80;
        if (marker != 0) {
            DisplayObject_SavePosition(marker);
            marker->f96 = 0;
        }
        D_801695D4 = gCampaignMap_MoveState;
        CampaignMap_StartCameraTween(
            gCampaignMap_Location, gCampaignMap_MoveState
        );
        if (gCampaignMap_Location < 10) {
            if (gCampaignMap_LocationPrev >= 10) {
                obj = D_801695D8;
                obj->f72 = 180;
                obj->f74 = 340;
                flags = D_801695EC;
                raise = obj->f8 | 0x40;
                D_801695EC = flags | 0x60;
                obj->f8 = raise;
            }
        }
        if (gCampaignMap_LocationPrev < 10 && gCampaignMap_Location >= 10) {
            D_801695EC |= 0x40;
        }
    }
    flags = D_801695EC;
    if ((flags & 0x40) != 0) {
        marker = D_801695D8;
        if ((flags & 0x20) == 0) {
            marker->f72 = marker->f72 + 7;
        } else {
            step = marker->f72 - 7;
            marker->f72 = step;
            if ((s16)step < 32) {
                marker->f72 = 32;
            }
        }
        marker->f74 = marker->f72 + 160;
    }
    marker = D_801695C8;
    if (marker != 0) {
        if (marker->f96 < 2048) {
            quotient = 2048 / gCampaignMap_MoveState;
            marker->f96 += quotient;
            DisplayObject_InterpolatePositionCosine(
                (DisplayObjectPosition *)marker,
                gCampaignMap_aLocationTable[
                    gCampaignMap_Location
                ].f12,
                gCampaignMap_aLocationTable[
                    gCampaignMap_Location
                ].f14,
                marker->f96
            );
        }
    }
    D_801695E4 = D_801695E4 + D_801695F0;
    D_801695E8 = D_801695E8 + D_801695F4;
    D_80169610 = D_80169610 + D_80169614;
    D_801695CC = D_801695CC + D_801695DC;
    D_801695D0 = D_801695D0 + D_801695E0;
    cam->angle = D_801695E4 >> 16;
    cam->field_04 = D_801695E8 >> 16;
    cam->field_00 = D_80169610 >> 16;
    cam->view.vrx = D_801695CC >> 16;
    cam->view.vrz = D_801695D0 >> 16;
    D_801695D4 = D_801695D4 - 1;
    if (D_801695D4 == 0) {
        CampaignMap_SetCameraFromLocation(gCampaignMap_Location);
        if ((D_801695EC & 0x20) != 0) {
            D_801695D8->f72 = 32;
            D_801695D8->f74 = 192;
        }
        if (D_801695C8 != 0) {
            D_801695C8->f48 =
                gCampaignMap_aLocationTable[
                    gCampaignMap_Location
                ].f12;
            D_801695C8->f50 =
                gCampaignMap_aLocationTable[
                    gCampaignMap_Location
                ].f14;
        }
    }
    func_8001352C();
    return D_801695D4;
}

s32 CampaignMap_PickExit(void)
{
    MapLocation *record;
    CampaignMapExit *exits;
    s32 ready;
    s32 i;

    record = gCampaignMap_aLocationTable + gCampaignMap_Location;
    exits = record->exits;
    if (gCampaignMap_Location >= 10) {
        if (Campaign_TestStoryFlag(CAMPAIGN_FLAG_TOURNAMENT_COMPLETE) != 0 &&
            (gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
            SD_SEPlayFull(48);
            gCampaignMap_MoveState = 24;
            return 0;
        }
    }
    if ((gInput_wPad1Pressed & PAD_BUTTON_CONFIRM_MASK) != 0) {
        ready = record->confirm_gate;
        if (ready != 0) {
            if (Campaign_TestStoryFlag(exits->story_flag) != 0) {
                ready = 0;
            }
        }
        if (ready == 0) {
            if (record->confirm_destination != 0) {
                gCampaignMap_MoveState = 24;
                SD_SEPlayFull(48);
                return record->confirm_destination;
            }
            SD_SEPlayFull(48);
            return gCampaignMap_Location | 0x8000;
        }
    }
    for (i = 0; i < 4; i++) {
        if (exits->destination != 16) {
            if (exits->story_flag == 0 ||
                Campaign_TestStoryFlag(exits->story_flag) != 0) {
                if ((gInput_wPad1Held & exits->input_mask) != 0) {
                    gCampaignMap_MoveState = exits->move_steps;
                    SD_SEPlayFull(6);
                    return exits->destination;
                }
            }
        }
        exits++;
    }
    return -1;
}

void CampaignMap_UpdateLocation(void)
{
    MapLocation *table;
    MapLocation *record;
    s32 exit;

    if (D_801695EC != 0) {
        if (CampaignMap_UpdateLocationTransition() != 0) {
            return;
        }
        D_801695EC = 0;
        CampaignMap_RebuildLocationObjects(gCampaignMap_Location);
        CampaignMap_CreateLocationLabel(gCampaignMap_Location);
        if (gCampaignMap_Location >= 10) {
            if (D_801695C8 == 0) {
                D_801695C8 = (MapObject *)
                    CampaignMap_CreateLocationMarker(gCampaignMap_Location);
            }
            table = gCampaignMap_aLocationTable;
            record = table + gCampaignMap_Location;
            D_801695C8->f48 = (u16)record->f12;
            D_801695C8->f50 = (u16)record->f14;
        } else {
            func_8004036C(D_801695C8);
            D_801695C8 = 0;
        }
    }
    if (D_8016960D != 0) {
        if ((D_8016960D & 0x80) == 0) {
            D_8016960D = D_8016960D | 0x80;
            Fade_InitOut();
            D_800E9ECF = 2;
            CampaignMap_ClearLocationObjects();
            func_8004036C(D_801695C8);
            SD_BGMFadeOutWithStep(4);
        }
        D_800F2848.field_00 = D_800F2848.field_00 - 2;
        func_8001352C();
        if ((D_800E9ECE & 0x80) == 0) {
            D_8009B26C = 2;
            D_8009B27A = gCampaignMap_Location + 32;
        }
        return;
    }
    gCampaignMap_LocationPrev = gCampaignMap_Location;
    exit = CampaignMap_PickExit();
    if (exit < 0) {
        return;
    }
    if ((exit & 0x8000) != 0) {
        D_8016960D = 1;
        return;
    }
    gCampaignMap_Location = exit;
    if ((u8)exit < 10) {
        func_8004036C(D_801695C8);
        D_801695C8 = 0;
    }
    CampaignMap_ClearLocationObjects();
    D_801695EC = 1;
}
