#ifndef YUGIOH_GAME_DISPLAY_OBJECT_PACKET_SUBMIT_H
#define YUGIOH_GAME_DISPLAY_OBJECT_PACKET_SUBMIT_H

#include "../types.h"
#include "sprite_primitive.h"

/* The packet dispatcher receives a sprite pointer in the sprite renderers,
 * while the Gouraud-quad renderers pass the display object's raw attribute
 * word. Keep both measured five-argument caller views behind one checked
 * owning header until func_80042188 itself is decompiled. */
#ifdef DISPLAY_OBJECT_PACKET_SUBMIT_RAW_ATTRIBUTE
void func_80042188(s32 attribute, u8 *packet, s32 ot, s32 mode, u8 *extra);
#else
void func_80042188(SpritePrim *sprite, u8 *packet, s32 ot, s32 mode, u8 *extra);
#endif

#endif
