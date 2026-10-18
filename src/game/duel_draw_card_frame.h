#ifndef MEMORIES_DECOMP_DUEL_DRAW_CARD_FRAME_H
#define MEMORIES_DECOMP_DUEL_DRAW_CARD_FRAME_H

#include "../types.h"
#include "../ygo_types.h"
#include "display_object.h"
#include "display_object_packet_submit.h"

/* The scratch record at 0x1F8003E0. DisplayObject_ProjectRotationFacing
   projects into its head and DisplayObject_SubmitPacket reads that head back as
   the packet origin;
   Duel_DrawCardFrame keeps the card's screen position in the word after it. */
typedef struct {
    DisplayObjectPacketOrigin origin;
    SpritePos pos;
} CardFrameScratch;

/* Draws one duel card's frame from its display object.
 *
 * The record is the canonical DisplayObject: this function reads the
 * attribute at 0x04, the flags at 0x08, the colour word at 0x0C, 0x14, the
 * word at 0x20 and the four tail bytes 0x67 to 0x6A, and every one of those
 * offsets is already named on that record. 0x6A is the index into
 * gDuel_aCardRecords that func_80017F04 (func_80017DB4.c) writes there, which is what
 * says this object is a duel card's.
 *
 * The second argument is the selected GsOT descriptor, passed straight to
 * the typed packet dispatcher. The projected-card caller supplies its
 * existing D_800E9D98 entry directly; the secondary-callback wrapper keeps
 * its word-shaped API and converts only at this boundary. Position widths
 * and the wrapper's partial first-object view remain unchanged. */
void Duel_DrawCardFrame(DisplayObject *object, GsOT *ordering_table, s32 arg2, s32 arg3);

#endif
