#include "../../types.h"
#include "../../game/model.h"
#include "camera_state.h"
#include "camera_transition.h"
#include "location_objects.h"
#include "location_marker.h"
#include "../../game/campaign_flags.h"
#include "../../game/model_slot_control.h"
#include "../../game/display_object_system.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/sound.h"
#include "../../game/duel_effect_text.h"
#include "../../game/input.h"
#include "../../game/sorted_entry_render.h"
#include "../../game/trig_constants.h"
#include "../../game/fade.h"
#include "campaign_map.h"
#include "../../game/view_state_orbit.h"
#include "../../game/main_services.h"
#include "../../game/model_slot_support.h"

#include "../../game/high_memory_addresses.h"
#include "../../game/shared_work_base.h"
#include "../../game/main_mode_state.h"

/* The live map state, zero-initialized and common (this unit builds with
   gcc_2_8_1_g0_split_comm); each layout's set_location_commons subsegment
   places it in that image's order. */
DisplayObject *gCampaignMap_pLocationMarker;
s32 gCampaignMap_dwTweenRefX;
s32 gCampaignMap_dwTweenRefZ;
s32 gCampaignMap_nTweenFramesLeft;
DisplayObject *gCampaignMap_pSpotlight;
s32 gCampaignMap_dwTweenRefXStep;
s32 gCampaignMap_dwTweenRefZStep;
s32 gCampaignMap_dwTweenYaw;
s32 gCampaignMap_dwTweenPitch;
u8 gCampaignMap_bTransitionFlags;
s32 gCampaignMap_dwTweenYawStep;
s32 gCampaignMap_dwTweenPitchStep;
DisplayObject *gCampaignMap_apExitObjects[CAMPAIGN_MAP_LOCATION_OBJECT_COUNT];
s32 gCampaignMap_MoveState;
u8 gCampaignMap_Location;
u8 gCampaignMap_bEnterSceneState;
s32 gCampaignMap_dwTweenDistance;
s32 gCampaignMap_dwTweenDistanceStep;
u8 gCampaignMap_LocationPrev;
u8 D_80169619;

void CampaignMap_ClearLocationObjects(void)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        DisplayObject_ReleaseIfPresent(gCampaignMap_apExitObjects[i]);
        gCampaignMap_apExitObjects[i] = 0;
    }
}

void CampaignMap_RebuildLocationObjects(s32 index)
{
    MapLocation *record;
    CampaignMapExit *entry;
    DisplayObject *object;
    s32 i;

    CampaignMap_ClearLocationObjects();
    record = gCampaignMap_aLocationTable + index;
    for (i = 0; i < 4; i++) {
        entry = &record->exits[i];
        if (entry->destination != 0x10) {
            if (entry->story_flag == 0 ||
                Campaign_TestStoryFlag(entry->story_flag) != 0) {
                object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                DisplayObject_ConfigureSpriteAtPositionWithResource(
                    object, entry->x, entry->y, 0, 2,
                    entry->direction_variant, 0x17, 0x100, gDisplay_abResourceBank0
                );
                DisplayObject_SetDepthOffset(object, 5);
                object->flags |=
                    DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
                gCampaignMap_apExitObjects[i] = object;
            }
        }
    }
}

#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
#define CAMPAIGN_MAP_LABEL_RENDER_MODE 0x10
#else
#define CAMPAIGN_MAP_LABEL_RENDER_MODE 0xC
#endif
struct DuelEffectChannel *CampaignMap_CreateLocationLabel(s32 unused)
{
    struct DuelEffectChannel *object;

    object = TextBox_Create(
        0, gCampaignMap_Location + 0x8350, 0x60, 0x18, 0x80,
        CAMPAIGN_MAP_LABEL_RENDER_MODE
    );
    TextBox_BuildBlockingWithGlyphScratch(object);
    return object;
}

#undef CAMPAIGN_MAP_LABEL_RENDER_MODE

void CampaignMap_SetCameraFromLocation(s32 index)
{
    MapLocation *entry = gCampaignMap_aLocationTable + index;
    ViewState *camera = &gViewState_Current;

    camera->orbit_angle = entry->camera_orbit_angle;
    camera->angle = entry->camera_angle;
    camera->distance = entry->camera_distance;
    camera->view.vrx = entry->view_x;
    camera->view.vrz = entry->view_z;
    ViewState_ApplyOrbit();
}

void CampaignMap_UpdateView(void)
{
    ViewState *camera = &gViewState_Current;
    u32 flags;

    SetGeomScreen(camera->projection);
    SetGeomOffset(0xA0, 0x78);
    SetFarColor(0, 0, 0);
    SetFogNearFar(0x7D0, 0x960, camera->projection);

    flags = gModel_dwFogDrawFlags;
    if (flags & 2) {
        gSortedEntry_nCount += 0xA;
        if (gSortedEntry_nCount >= gSortedEntry_nSortedCount) {
            gModel_dwFogDrawFlags = flags & ~3;
        }
        gSortedEntry_pCursor = gSortedEntry_pBase;
    }
    Model_DrawSlot(2);
}

void CampaignMap_ResetCamera(void)
{
    ViewState *camera = &gViewState_Current;
    /* A second pointer, to the view itself: storing the three reference
       coordinates through camera->view instead drops an instruction. */
    GsRVIEW2 *view = &gViewState_Current.view;

    camera->distance = 0x6A4;
    camera->angle = 0x640;
    camera->orbit_angle = 0x180;
    camera->field_0C = 0;
    camera->view.rz = 0;
    camera->view.super = 0;
    camera->projection = MODEL_DEFAULT_PROJECTION;
    GsSetProjection(MODEL_DEFAULT_PROJECTION);
    view->vrx = 0;
    camera->field_06 = 0;
    view->vry = 0;
    camera->field_08 = 0;
    view->vrz = 0;
    camera->field_0A = 0;
    ViewState_ApplyOrbit();
}

#ifdef VERSION_JAPAN
#define CAMPAIGN_MAP_CAMERA_FAST_BUTTON PAD_BUTTON_CIRCLE
#else
#define CAMPAIGN_MAP_CAMERA_FAST_BUTTON PAD_BUTTON_CROSS
#endif
void CampaignMap_MoveCameraDpad(void)
{
    ViewState *camera = &gViewState_Current;
    s32 step;

    if ((gInput_awPadHeld[0] &
         (PAD_DIRECTION_MASK | PAD_BUTTON_L1_R1_MASK)) != 0) {
        if ((gInput_awPadHeld[0] & PAD_BUTTON_TRIGGER_MASK) != 0) {
            if ((gInput_awPadHeld[0] & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
                if ((gInput_awPadHeld[0] & CAMPAIGN_MAP_CAMERA_FAST_BUTTON) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((gInput_awPadHeld[0] & PAD_DIRECTION_LEFT) != 0) {
                    step = -step;
                }
                camera->view.vrx = camera->view.vrx + step;
            }
            if ((gInput_awPadHeld[0] & PAD_DIRECTION_VERTICAL_MASK) != 0) {
                if ((gInput_awPadHeld[0] & CAMPAIGN_MAP_CAMERA_FAST_BUTTON) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((gInput_awPadHeld[0] & PAD_DIRECTION_DOWN) != 0) {
                    step = -step;
                }
                camera->view.vrz = camera->view.vrz + step;
            }
        } else {
            if ((gInput_awPadHeld[0] & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
                if ((gInput_awPadHeld[0] & CAMPAIGN_MAP_CAMERA_FAST_BUTTON) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((gInput_awPadHeld[0] & PAD_DIRECTION_LEFT) != 0) {
                    step = -step;
                }
                camera->angle = camera->angle + step;
            }
            if ((gInput_awPadHeld[0] & PAD_DIRECTION_VERTICAL_MASK) != 0) {
                if ((gInput_awPadHeld[0] & CAMPAIGN_MAP_CAMERA_FAST_BUTTON) != 0) {
                    step = 32;
                } else {
                    step = 2;
                }
                if ((gInput_awPadHeld[0] & PAD_DIRECTION_DOWN) != 0) {
                    step = -step;
                }
                camera->orbit_angle = camera->orbit_angle + step;
            }
            if ((gInput_awPadHeld[0] & PAD_BUTTON_L1_R1_MASK) != 0) {
                if ((gInput_awPadHeld[0] & CAMPAIGN_MAP_CAMERA_FAST_BUTTON) != 0) {
                    step = 20;
                } else {
                    step = 4;
                }
                if ((gInput_awPadHeld[0] & PAD_BUTTON_R1) != 0) {
                    step = -step;
                }
                camera->distance = camera->distance + step;
            }
        }
        ViewState_ApplyOrbit();
    }
}
#undef CAMPAIGN_MAP_CAMERA_FAST_BUTTON

DisplayObject *CampaignMap_CreateLocationMarker(s32 index)
{
    DisplayObject *object;
    MapLocation *table;
    MapLocation *record;
    MapLocation *entry;

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    table = gCampaignMap_aLocationTable;
    record = table + gCampaignMap_Location;
    DisplayObject_ConfigureSpriteAtPositionWithResource(
        object, record->marker_x, record->marker_y, 0, 1, 0,
        0x17, 0x100, gDisplay_abResourceBank0
    );
    DisplayObject_SetDepthOffset(object, 0xA);
    object->flags |=
        DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
        DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    entry = table + index;
    object->xy.h.x = entry->marker_x;
    object->xy.h.y = entry->marker_y;
    return object;
}

void CampaignMap_SetLocation(s32 index)
{
    struct _GsCOORDUNIT *unit;
    DisplayObject *obj;
    s32 track;
    s32 i;
    s32 location;
    DisplayObject *marker;
    DisplayObject *panel;
    u16 flags;
    s32 colour[3];

    gCampaignMap_Location = index;
    D_80169619 = 0;
    gCampaignMap_bEnterSceneState = 0;
    gCampaignMap_bTransitionFlags = 0;
    gCampaignMap_pLocationMarker = 0;
    for (i = 3; i >= 0; i--) {
        gCampaignMap_apExitObjects[i] = 0;
    }
    Model_ResetSceneView();
    Model_ResetSceneState();
    Model_LoadSlot(2, gRuntime_pSharedWorkBase, 0x43000, 0);
    unit = Model_GetCurrentDataEntry(2);
    colour[0] = 1365;
    colour[1] = 1365;
    colour[2] = 1365;
    Model_UpdateCoordinateUnitMatrix(unit, (VECTOR *)colour);
    CampaignMap_ResetCamera();
    SetFarColor(0, 0, 0);
    SetFogNearFar(6000, 8000, gViewState_Current.projection);
    gMain_apFrameServiceCallbacks[3] = CampaignMap_UpdateView;
    Model_SetFogDrawFlags(0);
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPositionWithResource(obj, 96, 24, 0, 0, 0, 23, 256, gDisplay_abResourceBank0);
    obj->flags =
        obj->flags |
        DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
        DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    obj->xy.h.x = 160;
    obj->xy.h.y = 144;
    obj->field_60 = 128;
    obj->field_48.spotlight.inner = 32;
    obj->field_48.spotlight.outer = 192;
    obj->field_44.scale.x = 5120;
    obj->field_44.scale.y = 4096;
    DisplayObject_SetDepthOffset(obj, -10);
    location = gCampaignMap_Location;
    obj->field_4C.draw = DisplayObject_RenderSpotlightMask;
    gCampaignMap_pSpotlight = obj;
    CampaignMap_SetCameraFromLocation(location);
    CampaignMap_CreateLocationLabel(gCampaignMap_Location);
    CampaignMap_RebuildLocationObjects(gCampaignMap_Location);
    gCampaignMap_LocationPrev = gCampaignMap_Location;
    if (gCampaignMap_Location >= 10) {
        marker = CampaignMap_CreateLocationMarker(gCampaignMap_Location);
        panel = gCampaignMap_pSpotlight;
        flags = panel->flags;
        gCampaignMap_pLocationMarker = marker;
        panel->flags = flags & 0xFFBF;
    }
    track = 0x70A0;
    if (Campaign_TestStoryFlag(CAMPAIGN_FLAG_TOURNAMENT_COMPLETE) != 0) {
        track = 0x70B0;
    }
    SD_BGMPlay(track);
}

void CampaignMap_StartCameraTween(s32 index, s32 steps)
{
    ViewState *camera = &gViewState_Current;
    MapLocation *entry = gCampaignMap_aLocationTable + index;
    s32 x;
    s32 y;
    s32 angle;
    s32 refX;
    s32 refZ;
    s32 stepX;
    s32 stepY;
    s32 turn;
    s32 stepTurn;
    s32 stepRefX;
    s32 stepRefZ;

    x = camera->distance;
    stepX = ((entry->camera_distance - x) << 16) / steps;
    y = camera->orbit_angle;
    stepY = ((entry->camera_orbit_angle - y) << 16) / steps;
    angle = camera->angle;
    turn = (entry->camera_angle - angle) & TRIG_ANGLE_MASK;
    gCampaignMap_dwTweenYaw = (angle << 16) | 0x8000;
    gCampaignMap_dwTweenPitch = (y << 16) | 0x8000;
    gCampaignMap_dwTweenDistance = (x << 16) | 0x8000;
    refX = camera->view.vrx;
    refZ = camera->view.vrz;
    gCampaignMap_dwTweenRefX = (refX << 16) | 0x8000;
    gCampaignMap_dwTweenRefZ = (refZ << 16) | 0x8000;
    gCampaignMap_dwTweenDistanceStep = stepX;
    gCampaignMap_dwTweenPitchStep = stepY;
    if (turn >= TRIG_ANGLE_HALF_TURN) {
        turn -= TRIG_ANGLE_FULL_TURN - 1;
    }
    stepTurn = (turn << 16) / steps;
    stepRefX = ((entry->view_x - refX) << 16) / steps;
    stepRefZ = ((entry->view_z - refZ) << 16) / steps;
    gCampaignMap_dwTweenYawStep = stepTurn;
    gCampaignMap_dwTweenRefXStep = stepRefX;
    gCampaignMap_dwTweenRefZStep = stepRefZ;
}

s32 CampaignMap_UpdateLocationTransition(void)
{
    DisplayObject *obj;
    DisplayObject *marker;
    ViewState *cam;
    u16 flags;
    u16 raise;
    s16 step;
    s32 timer;
    s32 quotient;
    cam = &gViewState_Current;
    cam = &gViewState_Current;
    flags = gCampaignMap_bTransitionFlags;
    if ((flags & 0x80) == 0) {
        marker = gCampaignMap_pLocationMarker;
        gCampaignMap_bTransitionFlags = flags | 0x80;
        if (marker != 0) {
            DisplayObject_SavePosition(marker);
            marker->field_60 = 0;
        }
        gCampaignMap_nTweenFramesLeft = gCampaignMap_MoveState;
        CampaignMap_StartCameraTween(
            gCampaignMap_Location, gCampaignMap_MoveState
        );
        if (gCampaignMap_Location < 10) {
            if (gCampaignMap_LocationPrev >= 10) {
                obj = gCampaignMap_pSpotlight;
                obj->field_48.spotlight.inner = 180;
                obj->field_48.spotlight.outer = 340;
                flags = gCampaignMap_bTransitionFlags;
                raise = obj->flags | 0x40;
                gCampaignMap_bTransitionFlags = flags | 0x60;
                obj->flags = raise;
            }
        }
        if (gCampaignMap_LocationPrev < 10 && gCampaignMap_Location >= 10) {
            gCampaignMap_bTransitionFlags |= 0x40;
        }
    }
    flags = gCampaignMap_bTransitionFlags;
    if ((flags & 0x40) != 0) {
        marker = gCampaignMap_pSpotlight;
        if ((flags & 0x20) == 0) {
            marker->field_48.spotlight.inner = marker->field_48.spotlight.inner + 7;
        } else {
            step = marker->field_48.spotlight.inner - 7;
            marker->field_48.spotlight.inner = step;
            if (step < 32) {
                marker->field_48.spotlight.inner = 32;
            }
        }
        marker->field_48.spotlight.outer = marker->field_48.spotlight.inner + 160;
    }
    marker = gCampaignMap_pLocationMarker;
    if (marker != 0) {
        if (marker->field_60 < 2048) {
            quotient = 2048 / gCampaignMap_MoveState;
            marker->field_60 += quotient;
            DisplayObject_InterpolatePositionCosine(
                marker,
                gCampaignMap_aLocationTable[
                    gCampaignMap_Location
                ].marker_x,
                gCampaignMap_aLocationTable[
                    gCampaignMap_Location
                ].marker_y,
                marker->field_60
            );
        }
    }
    gCampaignMap_dwTweenYaw = gCampaignMap_dwTweenYaw + gCampaignMap_dwTweenYawStep;
    gCampaignMap_dwTweenPitch = gCampaignMap_dwTweenPitch + gCampaignMap_dwTweenPitchStep;
    gCampaignMap_dwTweenDistance = gCampaignMap_dwTweenDistance + gCampaignMap_dwTweenDistanceStep;
    gCampaignMap_dwTweenRefX = gCampaignMap_dwTweenRefX + gCampaignMap_dwTweenRefXStep;
    gCampaignMap_dwTweenRefZ = gCampaignMap_dwTweenRefZ + gCampaignMap_dwTweenRefZStep;
    cam->angle = gCampaignMap_dwTweenYaw >> 16;
    cam->orbit_angle = gCampaignMap_dwTweenPitch >> 16;
    cam->distance = gCampaignMap_dwTweenDistance >> 16;
    cam->view.vrx = gCampaignMap_dwTweenRefX >> 16;
    cam->view.vrz = gCampaignMap_dwTweenRefZ >> 16;
    gCampaignMap_nTweenFramesLeft = gCampaignMap_nTweenFramesLeft - 1;
    if (gCampaignMap_nTweenFramesLeft == 0) {
        CampaignMap_SetCameraFromLocation(gCampaignMap_Location);
        if ((gCampaignMap_bTransitionFlags & 0x20) != 0) {
            gCampaignMap_pSpotlight->field_48.spotlight.inner = 32;
            gCampaignMap_pSpotlight->field_48.spotlight.outer = 192;
        }
        if (gCampaignMap_pLocationMarker != 0) {
            gCampaignMap_pLocationMarker->xy.h.x =
                gCampaignMap_aLocationTable[
                    gCampaignMap_Location
                ].marker_x;
            gCampaignMap_pLocationMarker->xy.h.y =
                gCampaignMap_aLocationTable[
                    gCampaignMap_Location
                ].marker_y;
        }
    }
    ViewState_ApplyOrbit();
    return gCampaignMap_nTweenFramesLeft;
}

#ifdef VERSION_JAPAN
#define CAMPAIGN_MAP_CANCEL_BUTTON PAD_BUTTON_CROSS
#define CAMPAIGN_MAP_CONFIRM_BUTTON_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#else
#define CAMPAIGN_MAP_CANCEL_BUTTON PAD_BUTTON_CANCEL
#define CAMPAIGN_MAP_CONFIRM_BUTTON_MASK PAD_BUTTON_CONFIRM_MASK
#endif
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
            (gInput_awPadPressed[0] & CAMPAIGN_MAP_CANCEL_BUTTON) != 0) {
            SD_SEPlayFull(48);
            gCampaignMap_MoveState = 24;
            return 0;
        }
    }
    if ((gInput_awPadPressed[0] & CAMPAIGN_MAP_CONFIRM_BUTTON_MASK) != 0) {
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
                if ((gInput_awPadHeld[0] & exits->input_mask) != 0) {
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

#undef CAMPAIGN_MAP_CANCEL_BUTTON
#undef CAMPAIGN_MAP_CONFIRM_BUTTON_MASK

void CampaignMap_UpdateLocation(void)
{
    MapLocation *table;
    MapLocation *record;
    s32 exit;

    if (gCampaignMap_bTransitionFlags != 0) {
        if (CampaignMap_UpdateLocationTransition() != 0) {
            return;
        }
        gCampaignMap_bTransitionFlags = 0;
        CampaignMap_RebuildLocationObjects(gCampaignMap_Location);
        CampaignMap_CreateLocationLabel(gCampaignMap_Location);
        if (gCampaignMap_Location >= 10) {
            if (gCampaignMap_pLocationMarker == 0) {
                gCampaignMap_pLocationMarker =
                    CampaignMap_CreateLocationMarker(gCampaignMap_Location);
            }
            table = gCampaignMap_aLocationTable;
            record = table + gCampaignMap_Location;
            gCampaignMap_pLocationMarker->xy.h.x = record->marker_x;
            gCampaignMap_pLocationMarker->xy.h.y = record->marker_y;
        } else {
            DisplayObject_ReleaseIfPresent(gCampaignMap_pLocationMarker);
            gCampaignMap_pLocationMarker = 0;
        }
    }
    if (gCampaignMap_bEnterSceneState != 0) {
        if ((gCampaignMap_bEnterSceneState & 0x80) == 0) {
            gCampaignMap_bEnterSceneState = gCampaignMap_bEnterSceneState | 0x80;
            Fade_InitOut();
            gFade_State.step = 2;
            CampaignMap_ClearLocationObjects();
            DisplayObject_ReleaseIfPresent(gCampaignMap_pLocationMarker);
            SD_BGMFadeOutWithStep(4);
        }
        gViewState_Current.distance = gViewState_Current.distance - 2;
        ViewState_ApplyOrbit();
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0) {
            gMain_bActiveModeState = MAIN_MODE_CAMPAIGN;
            gCampaignSceneIndex = gCampaignMap_Location + 32;
        }
        return;
    }
    gCampaignMap_LocationPrev = gCampaignMap_Location;
    exit = CampaignMap_PickExit();
    if (exit < 0) {
        return;
    }
    if ((exit & 0x8000) != 0) {
        gCampaignMap_bEnterSceneState = 1;
        return;
    }
    gCampaignMap_Location = exit;
    if (gCampaignMap_Location < 10) {
        DisplayObject_ReleaseIfPresent(gCampaignMap_pLocationMarker);
        gCampaignMap_pLocationMarker = 0;
    }
    CampaignMap_ClearLocationObjects();
    gCampaignMap_bTransitionFlags = 1;
}
