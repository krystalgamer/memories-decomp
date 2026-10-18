#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_PROJECTION_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_PROJECTION_H

#include "../types.h"

/* The projection result DisplayObject_ProjectRotationFacing writes for its
   caller. Every caller points this at a scratchpad address rather than at a
   local. */
struct ProjectionOut {
    s16 f0;
    s16 f2;
    s32 f4;
};

/* The record DisplayObject_ProjectRotationFacing projects is a display
   object. Only the tag is named here rather than including display_object.h:
   a pointer parameter needs no more, and an include on a header this widely
   consumed can reach an overlay source that carries a deliberate local
   declaration of its own. */
struct DisplayObject;

/* The facing test the Gouraud-quad list renderers run before submitting an
 * object with DISPLAY_OBJECT_FLAG_CLIP_TEST. It records the screen position
 * in `out`, sets it as the GTE geometry offset with the default projection
 * distance, and builds a ZYX rotation from the three bytes of the packed
 * field_20 word (each scaled by 16), with translation (0, 0, projection), as
 * the local-screen matrix. It then projects a reference triangle at the
 * origin, +X and +Y through RotAverageNclip3 and returns the signed NCLIP
 * area from MAC0, not OTZ. A non-positive result means the rotated plane
 * faces away, and the caller skips the object.
 *
 * The first parameter is u32, which is what the field the callers pass --
 * field_20.word -- already is. The matching Gouraud-quad list renderers
 * formerly declared it s32 and the fourth parameter u8 *, holding the
 * scratchpad address at 0x1F800398 as a byte pointer; both call sites now
 * state that conversion instead. */
s32 DisplayObject_ProjectPackedRotationFacing(u32 attribute, s32 x, s32 y,
                                              struct ProjectionOut *out);

/* The same facing test for one display object: the rotation comes from its
   field_20/21/22 bytes, field_65 goes into `out`, and the matrix is scaled by
   field_44/field_46 unless GsROTOFF is set. When the object has a callback in
   field_10, a negative (code-address) callback is called with the result,
   and a back-facing result mirrors the X and Z matrix columns and reprojects,
   so the back face is drawn mirrored. It returns the signed MAC0 result,
   non-positive when the object is skipped. The return type matters: one
   caller used to declare this `void`, which is invisible on this ABI because
   the result comes back in $v0 and nothing forces the caller to read it. */
s32 DisplayObject_ProjectRotationFacing(struct DisplayObject *obj, s32 x,
                                        s32 y, struct ProjectionOut *out);

#endif
