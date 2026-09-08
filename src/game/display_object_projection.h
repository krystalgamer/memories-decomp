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

/* Defined in display_object_projection.c. Deliberately incomplete here: the
   layout is that file's business, and a shared tag would let a caller pass an
   unrelated object that happens to be spelled `struct Obj` locally. */
struct ProjectionObj;

/* Projects one display object and returns its OTZ, or a non-positive value
   when the object is clipped. The return type matters: one caller used to
   declare this `void`, which is invisible on this ABI because the result
   comes back in $v0 and nothing forces the caller to read it. */
s32 func_80041F90(struct ProjectionObj *obj, s32 x, s32 y,
                  struct ProjectionOut *out);

#endif
