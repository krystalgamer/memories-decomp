#ifndef MEMORIES_DECOMP_SPRITE_PRIMITIVE_H
#define MEMORIES_DECOMP_SPRITE_PRIMITIVE_H

#include "../types.h"
#include "../ygo_types.h"

/* The sprite primitive the display-object paths build in the scratchpad at
   0x1F800320 before handing it to func_80042188. Four sources build one
   there; the address itself is reused for other things elsewhere
   (func_80015EF4.c reads it as a VECTOR, func_80016E70.c as a digit packet,
   func_80060B38.c as a GsSPRITE), so it is the type that identifies the
   object, not the address.
 
   It is a GsSPRITE (libgs.h) field for field, and the layout is not the only
   evidence for that. display_object_core.c initialises the object fields
   these are copied from with 0x00808080 and 0x10001000, which are r/g/b at
   0x80 each and scalex/scaley at 1.0 in 4.12; DisplayObject_RenderSpriteStrips advances `tpage`
   by a page step and bounds `uv.b.lo` at 0x100, which is what a u coordinate
   does; and DisplayObject_RenderSpriteList writes `rotate` only when the attribute's GsROTOFF
   bit is clear.
 
     0x00 attribute   0x0C tpage       0x18 mx, my
     0x04 x, y        0x0E u, v        0x1C scalex, scaley
     0x08 w, h        0x10 cx, cy      0x20 rotate
                      0x14 r, g, b
 
   It is spelt here rather than reused from libgs.h because the renderers
   reach several of these pairs as whole words - `xy`, `mxmy` and the `w`/`h`
   pair are each copied from the display object in one 32-bit move - while
   GsSPRITE spells every one of them as two separate members. The shared
   SpritePrim unions in ygo_types.h record both widths instead of forcing
   either. */

#endif
