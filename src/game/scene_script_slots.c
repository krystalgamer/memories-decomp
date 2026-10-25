#include "../types.h"
#include "scene_script.h"
#include "../psyq/libgte.h"
#include "trig_constants.h"
#include "display_object.h"
#include "scene_script_color_pulse.h"
#include "scene_script_noop_record_callback.h"
#include "scene_script_record_callbacks.h"

/* The two scene-script record callbacks, in the order gSceneScript_RecordCallbacks
   (scene_script_record_callbacks.c) holds them: SceneScript_UpdateSlots picks
   one by each live record's byte at +4. */

void SceneScript_NoOpRecordCallback(void)
{
}

#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->field_0C)

void SceneScript_UpdateColorPulse(SceneScriptSlot *state, DisplayObject *color)
{
    s32 angle;
    s32 intensity;
    s32 component;
    DisplayObject *object;

    if (((u16)state->callback_control & 0x8000) == 0) {
        state->callback_control |= 0x8000;
        state->pulse_angle = 0;
    }
    angle = ((u16)state->pulse_angle + 32) & TRIG_ANGLE_MASK;
    state->pulse_angle = angle;
    intensity = rsin(angle) * 24 / ONE;
    if (intensity == 0)
        intensity = 1;
    if (intensity == 24)
        intensity = 23;
    component = intensity - 104;
    DISPLAY_OBJECT_COLOR_BYTES(color)[2] = component;
    DISPLAY_OBJECT_COLOR_BYTES(color)[1] = component;
    DISPLAY_OBJECT_COLOR_BYTES(color)[0] = component;
    object = (DisplayObject *)gSceneScript_Slots[0].object_address;
    if (object != 0) {
        component = (intensity + 24) / 2 - 128;
        DISPLAY_OBJECT_COLOR_BYTES(object)[2] = component;
        DISPLAY_OBJECT_COLOR_BYTES(object)[1] = component;
        DISPLAY_OBJECT_COLOR_BYTES(object)[0] = component;
    }
    object = (DisplayObject *)gSceneScript_Slots[1].object_address;
    if (object != 0) {
        component = (intensity + 24) / 2 - 128;
        DISPLAY_OBJECT_COLOR_BYTES(object)[2] = component;
        DISPLAY_OBJECT_COLOR_BYTES(object)[1] = component;
        DISPLAY_OBJECT_COLOR_BYTES(object)[0] = component;
    }
}

void SceneScript_UpdateSlots(SceneScriptSlot *records) {
    s32 i;
    for (i = 0; i < 3; i++) {
        s32 value = records->object_address;
        if (value != 0) {
            gSceneScript_RecordCallbacks[*(u8 *)&records->callback_control](records, value);
        }
        records++;
    }
}
