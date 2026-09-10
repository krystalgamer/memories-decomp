#ifndef MEMORIES_DECOMP_MAIN_MENU_ORDERING_TABLES_H
#define MEMORIES_DECOMP_MAIN_MENU_ORDERING_TABLES_H

#include "../../types.h"
#include "../../psyq/libgs.h"

/* The ordering tables this overlay sorts its primitives into. frontend.c and
 * value_setup.c reach element 2 and trade_offers.c element 1.
 *
 * display_object.h already describes the array from the resident side: a
 * display object's field_16 is "an ordering-table index, not a texture index",
 * and func_80016D2C.c picks D_800E9D90[ot_index], casts that element to
 * GsOT * and hands it to GsSortFastSprite. That comment also notes that three
 * overlay files declare the array as GsOT *D_800E9D90[] -- these three.
 *
 * The typed spelling stays overlay-local on purpose. Resident units view the
 * same array as s32[], u8[] and u32 *[4], and none of them includes this
 * header, so the views never meet and no guarded arm is needed. Declaring the
 * GsOT * form where the resident spellings could see it would be a different
 * and much larger change: it would have to establish that every one of those
 * readings is the same object, which the resident comment records as an open
 * reading rather than a settled one. */
extern GsOT *D_800E9D90[];

#endif
