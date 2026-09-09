#include "../../types.h"
#include "../../unmatched.h"
#include "camera_transition.h"
#include "camera_state.h"
#include "../../game/trig_constants.h"
#include "../../game/view_state.h"
#include "../../game/func_80043178.h"
#include "../../game/display_object_interpolation.h"
#include "campaign_map.h"

typedef struct {
    u8 pad0[8];
    u16 f8;
    u8 pad10[38];
    s16 f48;
    s16 f50;
    u8 pad52[20];
    u16 f72;
    u16 f74;
    u8 pad76[20];
    s16 f96;
} MapObject;

typedef struct {
    u8 pad0[12];
    s16 f12;
    s16 f14;
    u8 pad16[50];
} Location;

extern u8 D_801695EC;
extern MapObject *D_801695C8;
extern MapObject *D_801695D8;
extern s32 D_801695D4;
extern s32 D_801695CC;
extern s32 D_801695D0;
extern s32 D_801695DC;
extern s32 D_801695E0;
extern s32 D_801695E4;
extern s32 D_801695E8;
extern s32 D_801695F0;
extern s32 D_801695F4;
extern s32 D_80169610;
extern s32 D_80169614;

void CampaignMap_StartCameraTween(s32 index, s32 steps)
{
    ViewState *camera = &D_800F2848;
    /* The same record through a word-sized base: retail reaches field_1C and
       field_24 off their own register while the first one is still holding
       the halfword fields. */
    s32 *cameraLong = (s32 *)&D_800F2848;
    u8 *entry = gCampaignMap_aLocationTable + index * 66;
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
    stepX = ((*(s16 *)(entry + 6) - x) << 16) / steps;
    y = camera->field_04;
    stepY = ((*(s16 *)(entry + 2) - y) << 16) / steps;
    angle = camera->angle;
    turn = (*(s16 *)(entry + 4) - angle) & TRIG_ANGLE_MASK;
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
    stepPitch = ((*(s16 *)(entry + 8) - pitch) << 16) / steps;
    stepDist = ((*(s16 *)(entry + 0xA) - dist) << 16) / steps;
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
            func_80043178(marker);
            marker->f96 = 0;
        }
        D_801695D4 = gCampaignMap_MoveState;
        CampaignMap_StartCameraTween(gCampaignMap_Location, gCampaignMap_MoveState);
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
            func_8004318C(
                marker,
                ((Location *)gCampaignMap_aLocationTable)[
                    gCampaignMap_Location
                ].f12,
                ((Location *)gCampaignMap_aLocationTable)[
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
    cam->field_1C = D_801695CC >> 16;
    cam->field_24 = D_801695D0 >> 16;
    D_801695D4 = D_801695D4 - 1;
    if (D_801695D4 == 0) {
        CampaignMap_SetCameraFromLocation(gCampaignMap_Location);
        if ((D_801695EC & 0x20) != 0) {
            D_801695D8->f72 = 32;
            D_801695D8->f74 = 192;
        }
        if (D_801695C8 != 0) {
            D_801695C8->f48 =
                ((Location *)gCampaignMap_aLocationTable)[
                    gCampaignMap_Location
                ].f12;
            D_801695C8->f50 =
                ((Location *)gCampaignMap_aLocationTable)[
                    gCampaignMap_Location
                ].f14;
        }
    }
    func_8001352C();
    return D_801695D4;
}
