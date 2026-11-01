#ifndef YUGIOH_GAME_DISPLAY_OBJECT_PACKET_SUBMIT_H
#define YUGIOH_GAME_DISPLAY_OBJECT_PACKET_SUBMIT_H

#include "../types.h"
#include "../ygo_types.h"
#include "sprite_primitive.h"

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* The screen origin DisplayObject_SubmitPacket subtracts from every vertex before the
 * perspective transform, followed by the subdivision count for textured
 * quads. A zero count sorts the quad as it is; anything else sends it
 * through DivideFT4. */
typedef struct {
    s16 x;
    s16 y;
    s32 divisions;
} DisplayObjectPacketOrigin;

/* The packet dispatcher receives a sprite pointer in the sprite renderers,
 * while the Gouraud-quad renderers pass the display object's raw attribute
 * word. The card-list renderer also preserves its named scratchpad records
 * rather than erasing them to byte pointers. All three measured five-argument
 * caller views stay behind this owning header; the definition in
 * display_object_helpers.c uses the default one and reads the first
 * argument either way, depending on the dispatch case. The card-list extra
 * is the same canonical origin/division record the dispatcher consumes;
 * its packet is the complete SDK textured quad, not only a header prefix. */
/* ot is the selected GsOT descriptor, not a depth, table index or texture.
 * Word-shaped wrapper APIs keep explicit pointer conversions at their calls. */
#ifdef DISPLAY_OBJECT_PACKET_SUBMIT_CARD_LIST
void DisplayObject_SubmitPacket(
    SpritePrim *sprite,
    POLY_FT4 *packet,
    GsOT *ot,
    s32 mode,
    DisplayObjectPacketOrigin *extra
);
#else
void DisplayObject_SubmitPacket(SpritePrim *sprite, u8 *packet, GsOT *ot, s32 mode, u8 *extra);
#endif

#endif
