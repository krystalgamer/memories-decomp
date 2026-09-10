#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_PROJECTION_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_PROJECTION_H

#include "../types.h"

/* The projection result func_80041F90 writes for its caller. Every caller
   points this at a scratchpad address rather than at a local. */
struct ProjectionOut {
    s16 f0;
    s16 f2;
    s32 f4;
};

/* The record func_80041F90 projects is a display object. Only the tag is
   named here rather than including display_object.h: a pointer parameter
   needs no more, and an include on a header this widely consumed can reach an
   overlay source that carries a deliberate local declaration of its own. */
struct DisplayObject;

/* The clip test the list renderers run before submitting an object: it takes
 * the object's field_20 word and a screen position, projects through the same
 * scratchpad ProjectionOut, and returns a positive value when the object
 * survives the test.
 *
 * The first parameter is u32, which is what the field the callers pass --
 * field_20.word -- already is. display_object_list_renderers.c declared it
 * s32 and the fourth parameter u8 *, holding the scratchpad address at
 * 0x1F800398 as a byte pointer; both call sites now state that conversion
 * instead. */
s32 func_80041E7C(u32 attribute, s32 x, s32 y, struct ProjectionOut *out);

/* Projects one display object and returns its OTZ, or a non-positive value
   when the object is clipped. The return type matters: one caller used to
   declare this `void`, which is invisible on this ABI because the result
   comes back in $v0 and nothing forces the caller to read it. */
s32 func_80041F90(struct DisplayObject *obj, s32 x, s32 y,
                  struct ProjectionOut *out);

#endif
