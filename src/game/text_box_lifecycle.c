#include "../types.h"
#include "duel_effect_init_entry.h"
#include "duel_effect_entry_occupancy.h"
#include "text_box_lifecycle.h"
#include "display_object_api.h"
#include "text_box_set_rect.h"

void TextBox_Destroy(void *record_ptr)
{
    DuelEffectChannel *record = record_ptr;

    func_80035CA8(record->index_57);
    DuelEffect_ClearMatchingMarker(record->index_57);
    record->flags_34 = 0;
    func_8004036C(record->field_30);
    func_8004036C(record->field_2C);
    func_8004036C(record->field_28);
    record->field_30 = 0;
    record->field_2C = 0;
    record->field_28 = 0;
}

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
    u8 *result;

    TextBox_SetRect(index, x, y, width, height);
    result = (u8 *)DuelEffect_InitEntry(index, string_id, 0);
    *(u16 *)(result + 0x34) |= flags;
    return result;
}
