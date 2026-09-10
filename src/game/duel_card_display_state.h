#ifndef MEMORIES_DECOMP_DUEL_CARD_DISPLAY_STATE_H
#define MEMORIES_DECOMP_DUEL_CARD_DISPLAY_STATE_H

#include "../types.h"
#include "duel_card.h"

/* The fields these helpers touch on a duel card's display object. This is not
 * the whole display record: the callers hold wider views of the same memory
 * under their own names. */
typedef struct {
    u8 pad_00[4];
    u8 field_04;
} DuelCardDisplayData;

/* Every field below is one func_80017F04, func_80017DB4, func_80017E3C or
   func_80018004 writes or reads on the object func_800400AC hands back. The
   offsets are unchanged; the fields added for func_80017F04 replace padding
   this struct already carried, so no existing user's codegen can move.

   Names are taken from records that already name the offset rather than
   invented here: attribute at 0x04 from the canonical DisplayObject in
   display_object.h, and out_x/out_y at 0x30/0x32 from DisplayObjectPosition
   in display_object_interpolation.h, where the interpolation helpers leave
   their eased result. func_80017F04 writes that pair as the object's initial
   position. The rest stay field_NN because no existing view names them:
   0x4C is a second callback slot, which DisplayObject covers with pad_4C,
   and all that is known here is that func_80017F04 stores func_80016D04
   into it. */
typedef struct {
    u8 pad_00[0x04];
    u32 attribute;
    u16 flags;
    u8 pad_0A[0x02];
    u32 color;
    void *field_10;
    u8 pad_14[0x0D];
    u8 field_21;
    u8 field_22;
    u8 pad_23[0x0D];
    s16 out_x;
    s16 out_y;
    s16 field_34;
    u8 pad_36[0x0C];
    u16 icon_variant;
    u8 pad_44[0x08];
    void *field_4C;
    u8 pad_50[0x0C];
    u8 icon_state;
    u8 field_5D;
    u8 pad_5E[0x09];
    u8 field_67;
    u8 field_68;
    u8 field_69;
    u8 card_index;
    u8 field_6B;
} DuelCardDisplayObject;

void func_80017DB4(DuelCardDisplayObject *object);
void func_80017E3C(DuelCardDisplayObject *object);

/* func_80018004 calls with the record alone so its incoming a1/a2 values pass
 * through unchanged. The arm below preserves that old-style call while every
 * ordinary caller gets the real three-argument prototype. */
#ifdef FUNC_80017F04_AMBIENT_POSITION_ARGS
u8 *func_80017F04();
#else
u8 *func_80017F04(DuelCardRecord *arg0, s32 arg1, s32 arg2);
#endif

#endif
