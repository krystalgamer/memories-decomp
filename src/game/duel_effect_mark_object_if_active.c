#include "../types.h"
#include "../ygo_types.h"
#include "menu_record.h"
#include "script_scene.h"
#include "duel_effect_mark_object_if_active.h"

/* Regional value: the flags_34 bit this sets (0x2 -> 0x4). The European
 * build (src/game/european/) defines its own. */
#ifndef DUEL_EFFECT_MARK_OBJECT_FLAG
#define DUEL_EFFECT_MARK_OBJECT_FLAG 2
#endif

/* Walks gDisplayEffect_aMenuRecords backward from the last record to the first; on the first
   record whose effect_id marker is non-negative, sets the flag in the
   caller's text-box channel's flags_34 and returns. Does nothing if every
   marker is negative. */
void DuelEffect_MarkObjectIfActive(DuelEffectChannel *channel) {
    int count;
    MenuRecord *p;
    int effect_id;

    count = MENU_RECORD_COUNT - 1;
    p = &gDisplayEffect_aMenuRecords[count];
    do {
        effect_id = p->effect_id;
        count -= 1;
        if (effect_id >= 0) {
            channel->flags_34 |= DUEL_EFFECT_MARK_OBJECT_FLAG;
            return;
        }
        p -= 1;
    } while (count >= 0);
}
