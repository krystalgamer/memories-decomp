/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * pinning 5 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/display_parent_links.c.
 */
#include "../types.h"
#include "../game/duel_grid.h"
#include "../game/display_object.h"
#include "../game/display_parent_links.h"
#include "../game/func_80022EEC.h"

void func_80022FF0(DisplayParent *argument, s32 flag)
{
    register DisplayParent *parent asm("$19") = argument;
    register int clear asm("$20") = flag;
    register DisplayLinkEntry *a asm("$17");
    register DisplayLinkEntry *b asm("$16");
    register int i asm("$18");

    a = parent->entries;
    if (a != 0) {
        i = 0;
        b = (DisplayLinkEntry *)((u8 *)a + 4);
        func_80022F98(parent, parent->base);
        parent->base = 0;
        do {
            func_80022F98(parent, a->object);
            func_80022F98(parent, b->object);
            if (clear) {
                a->object = 0;
                b->object = 0;
            }
            b++;
            i++;
            a++;
        } while (i < DUEL_FIELD_ROW_SIZE);
    }
}
