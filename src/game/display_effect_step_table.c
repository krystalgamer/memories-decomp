#include "../types.h"
#include "display_effect_step_table.h"
#include "func_8003A560.h"
#include "dialog_transition.h"
#include "display_effect_lifecycle.h"
#include "func_8003AC48.h"
#include "func_8003DA40.h"

/* Initialized data at 0x80090F68: the display effect step table.
 *
 * DisplayEffect_ProcessMenuRecords walks the menu records and, for each live
 * one, dispatches `display_effect_step & 0x1F`. The mask permits thirty-two
 * entries and there are thirteen, so an index above twelve would read past
 * the end into the table that follows. That is recorded rather than
 * corrected: nothing here shows the game produces such an index, and widening
 * the array would change the bytes.
 *
 * The entries are the display effect lifecycle handlers, including two that
 * share func_80039FF8. */

void func_8003A990(u8 *);
void func_8003AAE4(u8 *);
void func_8003AD6C(u8 *);
void func_8003B054(u8 *);

void (*D_80090F68[])(u8 *) = {
    (void (*)(u8 *))func_80039FF8,
    (void (*)(u8 *))func_80039FF8,
    (void (*)(u8 *))func_8003A560,
    func_8003A990,
    func_8003AD6C,
    func_8003B054,
    func_8003AC48,
    func_8003AAE4,
    func_8003DA40,
    func_8003D74C,
    func_8003D614,
    func_8003D518,
    func_8003DA40,
};
