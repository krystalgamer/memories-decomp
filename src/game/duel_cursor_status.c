#include "../types.h"
#include "duel_cursor_status.h"

extern void func_80023D08(DuelCursorStatus *);
extern void func_80023FBC(DuelCursorStatus *);

/* Neither caller consumes duel_cursor_status.h, and both reasons are
   deliberate.

   func_8001D5B4.c declares and calls func_80024088 with two arguments while
   the definition takes one; the second reaches $a1 and the call site's
   argument setup is what the image contains, so the true prototype would
   remove an instruction.

   duel_update_card_pick_cursor.c records that both callees are reached
   without a prototype in the original, so their results arrive in $v0
   already widened with no narrowing at the call site. Its own s32 return
   spelling preserves that.

   The one-parameter declaration of func_80023D08 above is also deliberate,
   and is the harder case of the two. The definition in func_80023D08.c takes
   (GridCursor *o, s32 dir) and reads dir four times, branching on its sign
   and on bits 0 and 1. func_80024088 sets only $a0: its own prologue is
   subu/sw/sw/jal with the object copy in the delay slot, and $a1 is never
   assigned, so dir arrives as whatever func_80024088's own caller happened to
   leave there.

   That is why this declaration cannot be widened to the true prototype. There
   is no expression to write for the second argument, because the call site
   does not compute one. Compare DisplayObject_ResetVelocity, where the same
   shape IS fixable: its callee needs only $a0 and $a0 already holds the
   object, so naming the argument costs nothing. The test is whether every
   register the callee reads holds a value this file can name. */

s32 func_80024060(DuelCursorStatus *object)
{
    func_80023FBC(object);
    return object->status;
}

s32 func_80024088(DuelCursorStatus *object)
{
    func_80023D08(object);
    return object->status;
}
