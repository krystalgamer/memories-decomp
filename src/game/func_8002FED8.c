#include "../types.h"
#include "scene_script.h"
#include "../psyq/libgte.h"
#include "trig_constants.h"
#include "display_object.h"
#include "func_8002FED8.h"

void func_8002FED8(SceneScriptSlot *state, DisplayObject *color)
{
    s32 angle;
    s32 intensity;
    s32 component;
    DisplayObject *object;

    if (((u16)state->unk04 & 0x8000) == 0) {
        state->unk04 |= 0x8000;
        state->unk06 = 0;
    }
    angle = ((u16)state->unk06 + 32) & TRIG_ANGLE_MASK;
    state->unk06 = angle;
    intensity = rsin(angle) * 24 / ONE;
    if (intensity == 0)
        intensity = 1;
    if (intensity == 24)
        intensity = 23;
    component = intensity - 104;
    ((u8 *)&color->field_0C)[2] = component;
    ((u8 *)&color->field_0C)[1] = component;
    ((u8 *)&color->field_0C)[0] = component;
    object = (DisplayObject *)D_800EAE98[0].unk00;
    if (object != 0) {
        component = (intensity + 24) / 2 - 128;
        ((u8 *)&object->field_0C)[2] = component;
        ((u8 *)&object->field_0C)[1] = component;
        ((u8 *)&object->field_0C)[0] = component;
    }
    object = (DisplayObject *)D_800EAE98[1].unk00;
    if (object != 0) {
        component = (intensity + 24) / 2 - 128;
        ((u8 *)&object->field_0C)[2] = component;
        ((u8 *)&object->field_0C)[1] = component;
        ((u8 *)&object->field_0C)[0] = component;
    }
}
