#include "../types.h"
#include "view_state.h"
#include "func_80022D94.h"
#include "../unmatched.h"

void Duel_StartViewTween(
    s32 frames, s32 distance, s32 orbit_angle, s32 heading, s32 target_z)
{
    s16 current_distance = D_800F2848.field_00;
    s32 distance_step = ((distance - current_distance) << 16) / frames;
    s16 current_orbit_angle = D_800F2848.field_04;
    s32 orbit_step = ((orbit_angle - current_orbit_angle) << 16) / frames;
    s16 current_heading = D_800F2848.angle;
    s32 heading_step = ((heading - current_heading) << 16) / frames;
    s32 current_target_z = D_800F2848.view.vrz;
    s32 target_z_step = ((target_z - current_target_z) << 16) / frames;

    gDuel_wViewTweenFramesRemaining = frames;
    D_8009B1EE = distance;
    D_8009B192 = orbit_angle;
    D_8009B190 = heading;
    D_8009B166 = target_z;
    D_8009B1C4 = (current_distance << 16) | 0x8000;
    D_8009B15C = (current_orbit_angle << 16) | 0x8000;
    D_8009B158 = (current_heading << 16) | 0x8000;
    D_8009B224 = (current_target_z << 16) | 0x8000;
    D_8009B1FC = distance_step;
    D_8009B198 = orbit_step;
    D_8009B194 = heading_step;
    D_8009B168 = target_z_step;
}
