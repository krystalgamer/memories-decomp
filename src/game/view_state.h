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
   the offset and the access width are measurements. `projection` is the one
   field whose MEANING is established rather than guessed: func_800178BC
   passes it straight to SetGeomScreen. The rest keep field_NN names because
   nothing in the matched code says what they are -- func_80022D94 calls the
   first three x, z and y while func_800178BC just numbers them, and there is
   no evidence here to settle that.

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
    s16 field_02;
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

#endif
