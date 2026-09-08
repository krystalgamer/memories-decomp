#include "../types.h"
#include "menu_record.h"

/* Walks D_800EB010 backward from the last record to the first; on the first
   record whose field_30 marker is non-negative, sets bit 0x2 in the caller's
   field_34 and returns. Does nothing if every marker is negative.

   The argument is another record from the same table, not a separate type:
   func_8002EB78 hands this routine's caller a D_800EB010 element and writes
   the same field_34 halfword itself. */
void DuelEffect_MarkObjectIfActive(MenuRecord *a0) {
    int count;
    MenuRecord *p;
    int v0;

    count = MENU_RECORD_COUNT - 1;
    p = &D_800EB010[count];
    do {
        v0 = p->field_30;
        count -= 1;
        if (v0 >= 0) {
            a0->field_34 |= 2;
            return;
        }
        p -= 1;
    } while (count >= 0);
}
