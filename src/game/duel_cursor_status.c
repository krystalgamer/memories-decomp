#define FUNC_80023D08_AMBIENT_DIRECTION_ARG
#include "../types.h"
#include "duel_cursor_status.h"
#include "func_80023D08.h"
#include "func_80023FBC.h"

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

   func_80024088's one-argument call to func_80023D08 is the harder case of
   the two, and it is now the FUNC_80023D08_AMBIENT_DIRECTION_ARG arm of
   func_80023D08.h rather than a private declaration here. The definition
   reads dir four times, branching on its sign and on bits 0 and 1, while
   func_80024088's prologue is subu/sw/sw/jal with the object copy in the
   delay slot and never assigns $a1: dir arrives as whatever func_80024088's
   own caller left there. There is no expression this file could write for it,
   because the call site does not compute one. Compare
   DisplayObject_ResetVelocity, where the same shape IS fixable: its callee
   needs only $a0 and $a0 already holds the object, so naming the argument
   costs nothing. The test is whether every register the callee reads holds a
   value this file can name. */

s32 func_80024060(DuelCursorStatus *object)
{
    func_80023FBC((GridCursor *)object);
    return object->status;
}

s32 func_80024088(DuelCursorStatus *object)
{
    func_80023D08((GridCursor *)object);
    return object->status;
}
