#include "../types.h"
#include "menu_record.h"
#include "display_menu_records.h"
#include "duel_effect_mark_object_if_active.h"

/* Regional value: the field_34 bit this sets (0x2 -> 0x4). The European
 * build (src/game/european/) defines its own. */
#ifndef DUEL_EFFECT_MARK_OBJECT_FLAG
#define DUEL_EFFECT_MARK_OBJECT_FLAG 2
#endif

/* Walks gDisplayEffect_aMenuRecords backward from the last record to the first; on the first
   record whose effect_id marker is non-negative, sets bit 0x2 in the caller's
   field_34 and returns. Does nothing if every marker is negative.

   The argument is another record from the same table, not a separate type:
   Script_OpShowMenu hands this routine's caller a gDisplayEffect_aMenuRecords element and writes
   the same field_34 halfword itself. */
void DuelEffect_MarkObjectIfActive(MenuRecord *a0) {
    int count;
    MenuRecord *p;
    int v0;

    count = MENU_RECORD_COUNT - 1;
    p = &gDisplayEffect_aMenuRecords[count];
    do {
        v0 = p->effect_id;
        count -= 1;
        if (v0 >= 0) {
            a0->field_34 |= DUEL_EFFECT_MARK_OBJECT_FLAG;
            return;
        }
        p -= 1;
    } while (count >= 0);
}
