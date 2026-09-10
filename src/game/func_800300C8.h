#ifndef MEMORIES_DECOMP_FUNC_800300C8_H
#define MEMORIES_DECOMP_FUNC_800300C8_H

#include "../types.h"

/* The two records func_800300C8 reads and writes. Both are narrow views: they
 * name only the fields this function touches and carry the rest as padding, so
 * neither is a claim about the whole record. */

/* Read side: a view of D_800EB15C, which duel_effect.h declares as the
 * DuelEffectChannel at D_800EB0F8[1]. Only the three values at 0x3C are
 * named, and the view stays because the two do not agree on +0x40: this
 * function reads it unsigned (`row`, u16) and DuelEffectChannel declares
 * the same halfword `s16 field_40`. Same offset, different signedness, so
 * it is not converted to the member; the pointer is cast at the use. */
struct LayoutSource {
    u8 pad0[0x3C];
    s16 base;
    s16 spacing;
    u16 row;
};

/* The write side, D_8009B2E4, used to have a narrow view here called
 * LayoutTarget. It is gone: the record is a DisplayObject.
 *
 * That view was declined a merge on the grounds that no file read both its
 * region (from 0x28) and the u16 at 0x08 that func_80031354.c named with a
 * local LocalRecord of its own. #3388 retired that LocalRecord for
 * DisplayObject, which names both -- flags at 0x08, and the four position
 * unions whose halfword pairs are exactly the eight fields LayoutTarget
 * listed: position.h at 0x28/0x2A, field_30.h at 0x30/0x32, field_38.h at
 * 0x38/0x3A and field_40.h at 0x40/0x42. So the overlapping witness the old
 * comment said did not exist is now in the tree, and the reason to keep a
 * separate view expired with it.
 *
 * LayoutSource above is NOT retired, for a reason that has not expired: it
 * and DuelEffectChannel disagree about the signedness of 0x40. */

void func_800300C8(void);

#endif
