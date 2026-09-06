#include "../../types.h"
#include "../../game/trig_constants.h"

extern u8 gCampaignMap_aLocationTable[];
extern u8 D_800F2848[];
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
    u8 *camera = D_800F2848;
    s32 *cameraLong = (s32 *)D_800F2848;
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

    x = *(s16 *)(camera + 0);
    stepX = ((*(s16 *)(entry + 6) - x) << 16) / steps;
    y = *(s16 *)(camera + 4);
    stepY = ((*(s16 *)(entry + 2) - y) << 16) / steps;
    angle = *(s16 *)(camera + 2);
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
