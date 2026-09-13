#ifndef YUGIOH_GAME_DISPLAY_OBJECT_PACKET_SUBMIT_H
#define YUGIOH_GAME_DISPLAY_OBJECT_PACKET_SUBMIT_H

#include "../types.h"
#include "../ygo_types.h"
#include "sprite_primitive.h"

/* The screen origin func_80042188 subtracts from every vertex before the
 * perspective transform, followed by the subdivision count for textured
 * quads. A zero count sorts the quad as it is; anything else sends it
 * through DivideFT4. */
typedef struct {
    s16 x;
    s16 y;
    s32 divisions;
} DisplayObjectPacketOrigin;

/* The libgs packet cursor (libgs.h's GsOUT_PACKET_P). DivideFT4 writes the
 * subdivided quads from it and returns the new cursor. Declared .data so a
 * -G8 unit addresses it absolutely, the way retail does. */
extern u8 *D_800FE240 __attribute__((section(".data")));

/* The packet dispatcher receives a sprite pointer in the sprite renderers,
 * while the Gouraud-quad renderers pass the display object's raw attribute
 * word. The card-list renderer also preserves its named scratchpad records
 * rather than erasing them to byte pointers. All three measured five-argument
 * caller views stay behind this owning header; the definition in
 * display_object_packet_submit.c uses the default one and reads the first
 * argument either way, depending on the dispatch case. */
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
