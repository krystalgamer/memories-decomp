#include "../types.h"
#include "duel_scene_callbacks.h"
#include "../unmatched.h"
#include "debug_effect_screen.h"
#include "duel_draw_resolution.h"
#include "func_8001825C.h"
#include "func_80019608.h"
#include "func_800208D4.h"
#include "func_80020F4C.h"

/* Initialized data at 0x80090998: the duel scene's phase callback table.
 *
 * func_80024200 in duel_scene_update.c materializes this address itself and
 * calls `callbacks[D_8009B23A & 0xF]()`, so the table is reached only through
 * the low four bits of that state word.
 *
 * It is written here rather than resolved out of the blob at 0x800908A0
 * because every entry is a function this tree already names: seven are
 * matching C and eight are still generated assembly. The unmatched ones are
 * declared in unmatched.h rather than here: this file only takes their
 * addresses, but that is the same thing the entries already in that header
 * do, so a local declaration would just be a second place for them to be
 * spelled.
 *
 * The table is fifteen entries and the mask permits sixteen. Index 15 would
 * read the first word of duel_terrain_boost, which begins immediately after
 * it. That is recorded rather than corrected: nothing here establishes that
 * the game ever produces index 15, and widening the array would change the
 * bytes. */

void func_80018608(void);
void func_8001898C(void);

void (*D_80090998[])(void) = {
    func_80022618,
    func_80018608,
    func_8001898C,
    func_80018DB4,
    func_8001BD88,
    func_8001D670,
    func_80019608,
    func_80019D18,
    func_8001B170,
    func_8001F55C,
    func_800208D4,
    func_8001825C,
    func_80020F4C,
    func_800218F0,
    func_80018FEC,
};
