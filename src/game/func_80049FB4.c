#include "../types.h"
#include "func_80049FB4.h"

s32 func_80049FB4(s32 note_high, s32 note_low, s32 base, s32 offset)
{
    s16 fine;
    s16 col;
    s16 diff;
    s16 down;
    s32 note;

    fine = note_low + offset;
    note = note_high + fine / 128;
    col = fine % 128;
    diff = note - base;
    if (diff >= 0) {
        return (u16)(D_80010834[diff % 12][col] << (diff / 12));
    }
    down = -diff;
    return (u16)(D_80010834[(12 - (s16)(down % 12)) % 12][col] >>
                 ((down + 11) / 12));
}
