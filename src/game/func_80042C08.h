#ifndef MEMORIES_DECOMP_FUNC_80042C08_H
#define MEMORIES_DECOMP_FUNC_80042C08_H

#include "../types.h"
#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Draws an elliptical spotlight mask of eight gouraud quads around the
 * object's +0x30 centre. Nothing calls it directly: dialog_transition.c and
 * the overworld's set_location.c store its address in a display object's
 * field_4C, and the object's draw pass supplies both arguments. */
void func_80042C08(DisplayObject *object, GsOT *ot);

#endif
