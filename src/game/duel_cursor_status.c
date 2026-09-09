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
   spelling preserves that. */

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
