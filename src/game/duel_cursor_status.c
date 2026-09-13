#define FUNC_80023D08_AMBIENT_DIRECTION_ARG
#include "../types.h"
#include "duel_cursor_status.h"
#include "func_80023D08.h"
#include "func_80023FBC.h"

/* The two callers consume different duel_cursor_status.h contracts, and both
   reasons are deliberate.

   func_8001D5B4.c selects FUNC_80024088_WIDE_DIRECTION and calls
   func_80024088 with two arguments while the definition takes one; the second
   reaches $a1 and the call site's argument setup is what the image contains,
   so the one-argument prototype would remove an instruction.

   duel_update_card_pick_cursor.c records that neither of its two call sites
   narrows the result -- in its listing each jal is followed by its delay slot
   and then a branch on $v0, with no andi or sll between -- so the widened
   value in $v0 is the callee's own. func_80024060 ends `lbu $v0, 0x19($s0)`
   and its value really is a byte; the s32 return spelling here preserves it
   widened, which is what that call site reads.

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
