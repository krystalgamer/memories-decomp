#ifndef MEMORIES_DECOMP_VIEW_STATE_H
#define MEMORIES_DECOMP_VIEW_STATE_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* The view state at D_800F2848. Three files used to declare three DIFFERENT
   structs for these same bytes -- `State { u16 a..h; char rest[0x20]; }` in
   view_state_reset_defaults.c, `Obj { s16 f0, f2, f4; u8 pad[8]; s16 fE; }` in
   the former duel_init_ground_projection_reference.c (now in
   duel_ground_projection.c) and
   `State { s16 x, y, z; char p6[0x1E]; int value; }` in
   duel_view_tween.c -- which is exactly the re-definition issue #2501 is about.
   They agree on the layout and disagree on the names and on where the record
   ends; this is the union of what they touch.

   Every offset below is read off a function that matches byte for byte, so
   the offset and the access width are measurements. Two fields already
   have semantic member names. `projection` is passed
   straight to SetGeomScreen by Duel_InitGroundProjectionReference. `angle` is the overworld
   camera's heading: CampaignMap_StartCameraTween takes the difference
   between it and the destination, masks that with TRIG_ANGLE_MASK and wraps
   it at TRIG_ANGLE_HALF_TURN against TRIG_ANGLE_FULL_TURN, which is modular
   arithmetic in the game's 0x1000-unit turn space and is not something done
   to a coordinate. ViewState_ResetDefaults corroborates it from the resident side and
   independently: it initialises the field to 0x400, which is exactly
   TRIG_ANGLE_QUARTER_TURN.

   That settles a question this header used to leave open. The former tween
   labels x, z and y were not evidence of coordinates. Duel_StartViewTween
   now names its distance, orbit-angle, heading and reference-target-Z
   channels from the producer/consumer operations. Heading interpolation
   remains linear rather than adding new wrapping behavior.

   Remaining field_NN names are retained pending complete cross-context
   naming. Both orbit bodies use field_00 as their radial magnitude; the
   shared field is also used by overworld code. The alternate orbit rearranges
   the angular roles, so field_04 is not globally renamed as pitch here.

   0x10..0x2F is a Psy-Q GsRVIEW2, and that is a call rather than an
   inference: ViewState_ApplyViewpointOffset builds a `GsRVIEW2 *` at exactly
   `&D_800F2848 + 0x10` and hands it to GsSetRefView2, whose parameter is that
   type. Its own arithmetic corroborates the member order -- it adds its
   three arguments to vrx, vry and vrz at 0x1C, 0x20 and 0x24 and writes the
   sums to vpx, vpy and vpz at 0x10, 0x14 and 0x18, which is what a reference
   view is. ViewState_ApplyViewpointOffset's header has said so since #2689;
   the record now spells it, and that file's note about not editing this
   header is gone with it.

   The old field_1C through field_2C are vrx, vry, vrz, rz and super under
   their Psy-Q names, at the same offsets and the same widths -- `long` is
   32-bit on this target, so nothing moves.

   Library_UpdateScreen reads the same range as two sixteen-byte blocks,
   `*(LibraryViewQuad *)&model->view.vpx` and `... &model->view.vry`. That
   is not a competing layout: a block copy of 0x10..0x2F says nothing about
   where the members inside it begin, and it stays spelled as its own cast
   in that file.

   There is no size assert: D_800F2848 is a single object with no stride
   evidence, so where the record ends is not something this code can show. */
typedef struct {
    s16 field_00;
    s16 angle;
    s16 field_04;
    s16 field_06;
    s16 field_08;
    s16 field_0A;
    s16 field_0C;
    s16 projection;
    GsRVIEW2 view;
} ViewState;

#define VIEW_STATE_OFFSET(member) ((u32)&(((ViewState *)0)->member))
typedef char ViewState_angle_offset_must_be_0x2[
    VIEW_STATE_OFFSET(angle) == 0x2 ? 1 : -1
];
typedef char ViewState_projection_offset_must_be_0xE[
    VIEW_STATE_OFFSET(projection) == 0xE ? 1 : -1
];
typedef char ViewState_view_offset_must_be_0x10[
    VIEW_STATE_OFFSET(view) == 0x10 ? 1 : -1
];
typedef char ViewState_vrx_offset_must_be_0x1C[
    VIEW_STATE_OFFSET(view.vrx) == 0x1C ? 1 : -1
];
typedef char ViewState_super_offset_must_be_0x2C[
    VIEW_STATE_OFFSET(view.super) == 0x2C ? 1 : -1
];
#undef VIEW_STATE_OFFSET

extern ViewState D_800F2848;

void ViewState_ApplyOrbit(void);
/* Address-based alias retained for the unmatched caller and its candidate. */


#endif
