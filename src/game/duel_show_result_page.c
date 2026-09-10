#include "../types.h"
#include "display_object_layout.h"
#include "display_object.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "display_object_config.h"
#include "duel_result_display.h"

void Duel_ShowResultPage(s32 page)
{
    s32 i;
    DisplayObject *child;
    void *object;

    func_80040410((DisplayObjectConfig *)D_8009B1E8->root, page);
    if (page == 0) {
        DuelResultDisplayState *state = D_8009B1E8;

        for (i = 0; i < DUEL_RESULT_DISPLAY_CHILD_COUNT; i++) {
            child = state->children[i];
            if (child == 0) {
                break;
            }
            child->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
    } else {
        DuelResultDisplayState *state = D_8009B1E8;

        for (i = 0; i < DUEL_RESULT_DISPLAY_CHILD_COUNT; i++) {
            child = state->children[i];
            if (child == 0) {
                break;
            }
            child->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
    }
    object = TextBox_Create(
        0, D_8009B1E8->page_text_ids[page], 0x1A, 0x28, 0x120, 0x120
    );
    func_80039A14(object);
}
