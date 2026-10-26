#ifndef MEMORIES_DECOMP_DUEL_CARD_DISPLAY_STATE_H
#define MEMORIES_DECOMP_DUEL_CARD_DISPLAY_STATE_H

#include "../types.h"
#include "duel_card.h"
#include "display_object_config.h"

#define DUEL_CARD_DISPLAY_FACE_DOWN_VALUE 0x80
#define DUEL_CARD_DISPLAY_DEFENSE_VALUE 0xC0

/* The fields these helpers touch on a duel card's display object. This is not
 * the whole display record: the callers hold wider views of the same memory
 * under their own names. */
/* Every field below is one Duel_CreateCardDisplayObject, Duel_UpdateCardDisplayMarker, Duel_UpdateCardDisplayState or
   Duel_CreateHandCardDisplayObject writes or reads on the object DisplayObject_AcquireSlot hands back. The
   offsets are unchanged; the fields added for Duel_CreateCardDisplayObject replace padding
   this struct already carried, so no existing user's codegen can move.

   Names are taken from records that already name the offset rather than
   invented here: attribute at 0x04 from the canonical DisplayObject in
   display_object.h, and out_x/out_y at 0x30/0x32 from DisplayObjectPosition
   in display_object_interpolation.h, where the interpolation helpers leave
   their eased result. Duel_CreateCardDisplayObject writes that pair as the object's initial
   position. The card-rendering metadata names the original-deck number
   marker at 0x67, cached card type at 0x68 and deck-entry index at 0x6B.
   Other views of these polymorphic bytes keep their own names. Offset
   0x4C is a second callback slot, which DisplayObject covers with pad_4C,
   and all that is known here is that Duel_CreateCardDisplayObject stores
   Duel_DrawCardAtOutputPosition into it. */
/* The +0x21/+0x22 bytes are the common display rotation_y/rotation_z.
   The two card-state helpers deliberately map card flags to them differently. */
typedef struct {
    u8 pad_00[0x04];
    u32 attribute;
    u16 flags;
    u8 pad_0A[0x02];
    u32 color;
    union {
        u32 word;
        void (*G32 callback)(DisplayObjectConfig *, u32);
    } field_10;
    u8 pad_14[0x0D];
    u8 rotation_y;
    u8 rotation_z;
    u8 pad_23[0x0D];
    s16 out_x;
    s16 out_y;
    s16 field_34;
    u8 pad_36[0x0C];
    u16 icon_variant;
    u8 pad_44[0x08];
    void *G32 field_4C;
    u8 pad_50[0x0C];
    u8 icon_state;
    u8 field_5D;
    u8 pad_5E[0x09];
    u8 deck_number_marker;
    u8 card_type;
    u8 field_69;
    u8 card_index;
    u8 deck_index;
} DuelCardDisplayObject;

void Duel_UpdateCardDisplayMarker(DuelCardDisplayObject *object);
void Duel_UpdateCardDisplayState(DuelCardDisplayObject *object);

/* Duel_CreateHandCardDisplayObject calls this through an unprototyped cast
 * with the record alone, so its incoming a1/a2 values pass through. */
DuelCardDisplayObject *Duel_CreateCardDisplayObject(DuelCardRecord *arg0, s32 arg1, s32 arg2);

#endif
