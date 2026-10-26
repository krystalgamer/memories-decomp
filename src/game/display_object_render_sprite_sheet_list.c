#include "../types.h"
#include "display_object.h"
#include "display_object_layout.h"
#define DISPLAY_OBJECT_UPDATE_COMMAND_STREAM_AMBIENT_ARGS
#include "display_object_update_command_stream.h"
#include "../unmatched.h"
#include "display_object_runtime.h"
#include "display_object_render_sprite_sheet_list.h"

#include "ordering_tables.h"

void DisplayObject_RenderSpriteSheetList(void) {
    s32 i = gDisplayObject_wSpriteSheetListHead;

    if (i >= 0) {
        DisplayObject *base = gDisplayObject_aPool;
        GsOT *G32 *t = gGraphics_apOrderingTables;

        do {
            DisplayObject *p =
                (DisplayObject *)(i * DISPLAY_OBJECT_RECORD_SIZE + (s32)base);
            DisplayObjectCallback f = p->update;
            u8 *q = (u8 *)p;

            i = p->next;

            if (f != 0) {
                f(q);
            }

            if (((p->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                 DISPLAY_OBJECT_RENDERABLE_MASK) == 0) {
                DisplayObject_UpdateCommandStream((DisplayObject *)q);
                DisplayObject_RenderSpriteSheet(
                    (DisplayObject *)q, t[p->ot_index],
                    (s16)p->ot_depth
                );
            }
        } while (i >= 0);
    }
}
