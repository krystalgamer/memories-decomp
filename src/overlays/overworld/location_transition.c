#include "../../types.h"

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    u8 pad6[22];
    s32 f28;
    u8 pad32[4];
    s32 f36;
} Camera;

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
extern u8 gCampaignMap_Location;
extern u8 gCampaignMap_LocationPrev;
extern s32 gCampaignMap_MoveState;
extern s32 D_801695D4;
extern Location gCampaignMap_aLocationTable[];
extern u8 D_800F2848[];

extern s32 D_801695E4;
extern s32 D_801695F0;
extern s32 D_801695E8;
extern s32 D_801695F4;
extern s32 D_80169610;
extern s32 D_80169614;
extern s32 D_801695CC;
extern s32 D_801695DC;
extern s32 D_801695D0;
extern s32 D_801695E0;

extern void func_80043178(MapObject *);
extern void CampaignMap_StartCameraTween(s32, s32);
extern void func_8004318C(MapObject *, s32, s32, s32);
extern void CampaignMap_SetCameraFromLocation(s32);
extern void func_8001352C(void);

s32 CampaignMap_UpdateLocationTransition(void)
{
    MapObject *obj;
    MapObject *marker;
    Camera *cam;
    u16 flags;
    u16 raise;
    s32 step;
    s32 timer;
    s32 quotient;

    cam = (Camera *)D_800F2848;
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
            func_8004318C(marker, gCampaignMap_aLocationTable[gCampaignMap_Location].f12, gCampaignMap_aLocationTable[gCampaignMap_Location].f14, marker->f96);
        }
    }
    D_801695E4 = D_801695E4 + D_801695F0;
    D_801695E8 = D_801695E8 + D_801695F4;
    D_80169610 = D_80169610 + D_80169614;
    D_801695CC = D_801695CC + D_801695DC;
    D_801695D0 = D_801695D0 + D_801695E0;
    cam->y = D_801695E4 >> 16;
    cam->z = D_801695E8 >> 16;
    cam->x = D_80169610 >> 16;
    cam->f28 = D_801695CC >> 16;
    cam->f36 = D_801695D0 >> 16;
    D_801695D4 = D_801695D4 - 1;
    if (D_801695D4 == 0) {
        CampaignMap_SetCameraFromLocation(gCampaignMap_Location);
        if ((D_801695EC & 0x20) != 0) {
            D_801695D8->f72 = 32;
            D_801695D8->f74 = 192;
        }
        if (D_801695C8 != 0) {
            D_801695C8->f48 = gCampaignMap_aLocationTable[gCampaignMap_Location].f12;
            D_801695C8->f50 = gCampaignMap_aLocationTable[gCampaignMap_Location].f14;
        }
    }
    func_8001352C();
    return D_801695D4;
}
