#ifndef MEMORIES_DECOMP_VIEW_STATE_H
#define MEMORIES_DECOMP_VIEW_STATE_H

#include "../types.h"

/* The view state at D_800F2848. Three files used to declare three DIFFERENT
   structs for these same bytes -- `State { u16 a..h; char rest[0x20]; }` in
   func_80017130.c, `Obj { s16 f0, f2, f4; u8 pad[8]; s16 fE; }` in
   func_800178BC.c and `State { s16 x, y, z; char p6[0x1E]; int value; }` in
   func_80022D94.c -- which is exactly the re-definition issue #2501 is about.
   They agree on the layout and disagree on the names and on where the record
   ends; this is the union of what they touch.

   Every offset below is read off a function that matches byte for byte, so
   the offset and the access width are measurements. Two fields have an
   established MEANING rather than a guessed one. `projection` is passed
   straight to SetGeomScreen by func_800178BC. `angle` is the overworld
   camera's heading: CampaignMap_StartCameraTween takes the difference
   between it and the destination, masks that with TRIG_ANGLE_MASK and wraps
   it at TRIG_ANGLE_HALF_TURN against TRIG_ANGLE_FULL_TURN, which is modular
   arithmetic in the game's 0x1000-unit turn space and is not something done
   to a coordinate. func_80017130 corroborates it from the resident side and
   independently: it initialises the field to 0x400, which is exactly
   TRIG_ANGLE_QUARTER_TURN.

   That settles a question this header used to leave open. func_80022D94
   called the first three x, z and y and func_800178BC just numbered them;
   both readings put a coordinate in the middle and the wrap rules both out.
   func_80022D94's own local for it is still spelled `oldY`, which is a name
   rather than evidence -- it interpolates the field linearly, which an angle
   tolerates, so nothing there contradicts this.

   The rest keep field_NN names because nothing in the matched code says what
   they are.

   0x10..0x1B is padding only in the sense that no matching function touches
   it FIELD BY FIELD. func_8002BAB4 copies it as two sixteen-byte blocks,
   `*(ViewQuad *)(model + 0x10)` and `*(ViewQuad *)(model + 0x20)`, so
   0x10..0x2F is really two quads and field_1C is the last word of the first
   rather than a loner. The offsets below are unchanged by that reading --
   it is a regrouping, not a correction -- so it is recorded here rather
   than acted on: restructuring would be a claim about meaning that the
   copy alone does not settle. There is no size assert: D_800F2848 is a single object with no stride
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
    u8 pad_10[0xC];
    s32 field_1C;
    s32 field_20;
    s32 field_24;
    s32 field_28;
    s32 field_2C;
} ViewState;

#define VIEW_STATE_OFFSET(member) ((u32)&(((ViewState *)0)->member))
typedef char ViewState_angle_offset_must_be_0x2[
    VIEW_STATE_OFFSET(angle) == 0x2 ? 1 : -1
];
typedef char ViewState_projection_offset_must_be_0xE[
    VIEW_STATE_OFFSET(projection) == 0xE ? 1 : -1
];
typedef char ViewState_field_1C_offset_must_be_0x1C[
    VIEW_STATE_OFFSET(field_1C) == 0x1C ? 1 : -1
];
typedef char ViewState_field_2C_offset_must_be_0x2C[
    VIEW_STATE_OFFSET(field_2C) == 0x2C ? 1 : -1
];
#undef VIEW_STATE_OFFSET

extern ViewState D_800F2848;

void func_8001352C(void);

#endif
