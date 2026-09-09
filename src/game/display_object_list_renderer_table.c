#include "../types.h"
#include "display_object_list_renderer_table.h"
#include "display_object_layout.h"

/* Initialized data at 0x80090FB0: the per-list display object renderers.
 *
 * This is the one table in this group with a real bound. func_80041340 walks
 * it with `for (i = 0; i < DISPLAY_OBJECT_LIST_COUNT; i++)`, that constant is
 * seven in display_object_layout.h, and there are seven entries. It agrees
 * independently with display_object.h, where the list heads are
 * `s16 D_800EFE38[DISPLAY_OBJECT_LIST_COUNT]`, and with
 * DisplayObject_ResetPool, which clears the same count.
 *
 * So this array is declared sized, unlike the other three, because the size
 * is established rather than assumed. */

void func_80040588(void);
void func_80040814(void);
void func_80040BF8(void);
void func_80040CAC(void);
void func_80040D14(void);
void func_80040DD8(void);
void func_80041068(void);

void (*D_80090FB0[DISPLAY_OBJECT_LIST_COUNT])(void) = {
    func_80040CAC,
    func_80040588,
    func_80040814,
    func_80040BF8,
    func_80040DD8,
    func_80041068,
    func_80040D14,
};
