#include "../types.h"
#include "duel_effect_init_entry.h"
#include "duel_effect_entry_occupancy.h"
#include "text_box_lifecycle.h"
#include "display_object_core.h"
#include "text_box_set_rect.h"
#ifdef VERSION_EUROPE
#include "duel_effect.h"
#include "text_channel_state.h"
#endif

void TextBox_Destroy(DuelEffectChannel *record)
{
#ifdef VERSION_EUROPE
    /* The European build leaves an inactive channel alone and no longer
       clears the channel's occupancy value. */
    if (record->flags_34 & DUEL_EFFECT_CHANNEL_FLAG_ACTIVE) {
        DuelEffect_ClearMatchingMarker(record->index_57);
        record->flags_34 = 0;
        DisplayObject_ReleaseIfPresent(record->prompt_object);
        DisplayObject_ReleaseIfPresent(record->aux_object);
        DisplayObject_ReleaseIfPresent(record->render_object);
        record->prompt_object = 0;
        record->aux_object = 0;
        record->render_object = 0;
    }
#else
    DuelEffect_ClearOccupancyValue(record->index_57);
    DuelEffect_ClearMatchingMarker(record->index_57);
    record->flags_34 = 0;
    DisplayObject_ReleaseIfPresent(record->prompt_object);
    DisplayObject_ReleaseIfPresent(record->aux_object);
    DisplayObject_ReleaseIfPresent(record->render_object);
    record->prompt_object = 0;
    record->aux_object = 0;
    record->render_object = 0;
#endif
}

#ifdef VERSION_EUROPE
/* European only: destroys the channel at an index into gText_aChannels. */
void TextBox_DestroyIndex(s32 index)
{
    TextBox_Destroy(&gText_aChannels[index]);
}
#endif

void *TextBox_Create(
    s32 index,
    s32 string_id,
    s32 x,
    s32 y,
    s32 width,
    s32 height
)
{
    TextBox_SetRect(index, x, y, width, height);
    return (u8 *)DuelEffect_InitEntry(index, string_id, 0);
}

void *TextBox_CreateFlagged(
    s32 index,
    s32 string_id,
    s32 x,
    s32 y,
    s32 width,
    s32 height,
    s32 flags
)
{
#ifdef VERSION_EUROPE
    /* The European build hands the flags to DuelEffect_InitEntry. */
    TextBox_SetRect(index, x, y, width, height);
    return DuelEffect_InitEntry(index, string_id, flags);
#else
    DuelEffectChannel *result;

    TextBox_SetRect(index, x, y, width, height);
    result = DuelEffect_InitEntry(index, string_id, 0);
    result->flags_34 |= flags;
    return result;
#endif
}
