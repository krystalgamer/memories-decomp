#include "../types.h"
#include "duel_grid.h"
#include "display_object.h"
#include "display_parent_links.h"
#include "duel_selection_update_linked_object.h"

void DuelSelection_LinkDisplayObject(
    DisplayParent *parent,
    volatile DisplayObject *object
)
{
    DisplayObject *base;
    int index;

    if (object != 0) {
        base = parent->position_base;
        object->position.h.field_28 = object->field_30.h.field_30 - base->field_30.h.field_30;
        object->position.h.field_2A = object->field_30.h.field_32 - base->field_30.h.field_32;
        index = parent->index;
        object->field_6C = 1;
        object->update =
            (DisplayObjectCallback)DuelSelection_UpdateLinkedObject;
        object->field_2C.h.field_2C = index;
    }
}

/*
 * Derive paired cursors inside the loop: `DuelSelection_LinkDisplayObjects`
 *
 * The parent-link routine at `0x80022FF0` matches all 160 bytes with the existing
 * uniform `gcc_2_8_1_g8` profile, without its historical five register pins.
 * Passing `parent` and `clear` directly, rather than copying the arguments into
 * locals, first removes the saved-register displacement. Maintaining two
 * independent 12-byte cursors still exchanges `$s0` and `$s1` at nine instructions.
 * Deriving the second object pointer at `entry + 4` inside each iteration lets
 * ordinary loop strength reduction allocate both cursors as retail does.
 *
 * The remaining two-word discriminator is the increment order:
 * `entry++, slot++` gives retail's second-cursor increment before its slot
 * increment. Reversing those source increments swaps instructions at `+0x6C`
 * and `+0x70`. The source configures the base object first, then both objects in
 * each of five entries, through `DuelSelection_LinkDisplayObject`; it always
 * clears `parent->base` and only clears the two entry pointers when requested.
 * The canonical history and six terminal refinement rows remain unchanged;
 * one post-terminal resolution records the newly verified source structure.
 */
void DuelSelection_LinkDisplayObjects(DisplayParent *parent, s32 clear)
{
    DisplayLinkEntry *entry;
    DisplayLinkEntry *child;
    s32 slot;

    entry = parent->entries;
    if (entry != 0) {
        DuelSelection_LinkDisplayObject(parent, parent->base);
        parent->base = 0;
        for (slot = 0; slot < DUEL_FIELD_ROW_SIZE; entry++, slot++) {
            child = (DisplayLinkEntry *)((u8 *)entry + 4);
            DuelSelection_LinkDisplayObject(parent, entry->object);
            DuelSelection_LinkDisplayObject(parent, child->object);
            if (clear) {
                entry->object = 0;
                child->object = 0;
            }
        }
    }
}
