/*
 * Derive paired cursors inside the loop: `func_80022FF0`
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
 * each of five entries, through `func_80022F98`; it always clears `parent->base`
 * and only clears the two entry pointers when requested.
 * The canonical history and six terminal refinement rows remain unchanged;
 * one post-terminal resolution records the newly verified source structure.
 */
#include "../types.h"
#include "duel_grid.h"
#include "display_parent_links.h"

void func_80022FF0(DisplayParent *parent, s32 clear)
{
    DisplayLinkEntry *entry;
    DisplayLinkEntry *child;
    s32 slot;

    entry = parent->entries;
    if (entry != 0) {
        func_80022F98(parent, parent->base);
        parent->base = 0;
        for (slot = 0; slot < DUEL_FIELD_ROW_SIZE; entry++, slot++) {
            child = (DisplayLinkEntry *)((u8 *)entry + 4);
            func_80022F98(parent, entry->object);
            func_80022F98(parent, child->object);
            if (clear) {
                entry->object = 0;
                child->object = 0;
            }
        }
    }
}
