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
 * word. The definition in display_object_system.c reads the packet and the
 * extra record as bytes because their type depends on the dispatch case, so
 * every caller passes byte pointers. For the card-list renderer the extra is
 * the DisplayObjectPacketOrigin record the dispatcher consumes, and its
 * packet is the complete SDK textured quad. */
/* ot is the selected GsOT descriptor, not a depth, table index or texture.
 * Word-shaped wrapper APIs keep explicit pointer conversions at their calls. */
void DisplayObject_SubmitPacket(SpritePrim *sprite, u8 *packet, GsOT *ot, s32 mode, u8 *extra);

#endif
