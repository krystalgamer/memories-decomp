#include "../types.h"
#include "display_object_list_renderer_table.h"
#include "display_object_layout.h"
#include "../unmatched.h"
#include "display_object_updates.h"
#include "display_object_render_sprite_list.h"
#include "func_80040814.h"

/* Initialized data at 0x80090FB0: the per-list display object renderers.
 *
 * This is the one table in this group with a real bound. DisplayObject_RenderFrame walks
 * it with `for (i = 0; i < DISPLAY_OBJECT_LIST_COUNT; i++)`, that constant is
 * seven in display_object_layout.h, and there are seven entries. It agrees
 * independently with display_object.h, where the list heads are
 * `s16 D_800EFE38[DISPLAY_OBJECT_LIST_COUNT]`, and with
 * DisplayObject_ResetPool, which clears the same count.
 *
 * So this array is declared sized, unlike the other three, because the size
 * is established rather than assumed. */

void (*gDisplayObject_ListRenderers[DISPLAY_OBJECT_LIST_COUNT])(void) = {
    func_80040CAC,
    DisplayObject_RenderSpriteList,
    func_80040814,
    func_80040BF8,
    DisplayObject_RenderGouraudQuadList,
    DisplayObject_RenderTexturedGouraudQuadList,
    func_80040D14,
};
