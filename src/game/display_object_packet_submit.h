#ifndef YUGIOH_GAME_DISPLAY_OBJECT_PACKET_SUBMIT_H
#define YUGIOH_GAME_DISPLAY_OBJECT_PACKET_SUBMIT_H

#include "../types.h"
#include "../ygo_types.h"
#include "sprite_primitive.h"

/* The packet dispatcher receives a sprite pointer in the sprite renderers,
 * while the Gouraud-quad renderers pass the display object's raw attribute
 * word. The card-list renderer also preserves its named scratchpad records
 * rather than erasing them to byte pointers. Keep all three measured
 * five-argument caller views behind one checked owning header until
 * func_80042188 itself is decompiled. */
#ifdef DISPLAY_OBJECT_PACKET_SUBMIT_RAW_ATTRIBUTE
void func_80042188(s32 attribute, u8 *packet, s32 ot, s32 mode, u8 *extra);
#elif defined(DISPLAY_OBJECT_PACKET_SUBMIT_CARD_LIST)
void func_80042188(
    SpritePrim *sprite,
    Func80028B08Ctx *packet,
    s32 ot,
    s32 mode,
    Func80028B08Extra *extra
);
#else
void func_80042188(SpritePrim *sprite, u8 *packet, s32 ot, s32 mode, u8 *extra);
#endif

#endif
