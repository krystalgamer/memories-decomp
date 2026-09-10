#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "model.h"
#include "view_state.h"
#include "view_state_orbit.h"

/* Placing the D_800F2848 view: func_800134E0 puts the viewpoint at an
   offset from the reference point and installs the GsRVIEW2, and the two
   after it compute that offset as an orbit - a distance from field_00
   turned through field_04 and angle with rsin/rcos - and hand it over.
   func_8001352C is the one the duel, Library and overworld code call after
   moving the view; func_800135FC is the same orbit with the two angles
   swapped, a quarter turn added to the second, and its last two offsets
   exchanged.

   The three sources were recorded at gcc_2_8_1_g8, gcc_2_8_1_g0_split and
   gcc_2_8_1_g8_split, and each compiles to an identical object at
   gcc_2_8_1_g8_split. Bounded below by main_services.c and above by the
   empty func_800136D4, which main_services.c calls and nothing here does. */

void func_800134E0(ViewState *object, s32 x, s32 y, s32 z)
{
    GsRVIEW2 *view = &object->view;

    x += view->vrx;
    view->vpx = x;
    y += view->vry;
    z += view->vrz;
    view->vpy = y;
    view->vpz = z;
    GsSetRefView2(view);
}

void func_8001352C(void) {
    ViewState *p = &D_800F2848;
    s32 n = -D_800F2848.field_00;
    s32 x = n * rcos(p->field_04) / MODEL_FIXED_ONE;
    s32 z = n * rsin(p->field_04) / MODEL_FIXED_ONE;
    s32 y = x * rsin(p->angle) / MODEL_FIXED_ONE;
    s32 w = x * rcos(p->angle) / MODEL_FIXED_ONE;

    x = w;
    func_800134E0(p, x, z, y);
}

void func_800135FC(void)
{
    ViewState *record = &D_800F2848;
    s32 distance = -record->field_00;
    s32 x = distance * rcos(record->angle) / MODEL_FIXED_ONE;
    s32 z = distance * rsin(record->angle) / MODEL_FIXED_ONE;
    s32 y = x * rsin(
        record->field_04 + MODEL_ANGLE_QUARTER_TURN
    ) / MODEL_FIXED_ONE;
    s32 w = x * rcos(
        record->field_04 + MODEL_ANGLE_QUARTER_TURN
    ) / MODEL_FIXED_ONE;

    x = w;
    func_800134E0(record, x, y, z);
}
