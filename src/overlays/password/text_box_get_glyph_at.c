#include "../../types.h"
#include "name_entry_keyboard.h"

DuelEffectEntry *TextBox_GetGlyphAt(s32 index, s32 x, s32 y)
{
    DuelEffectChannel *base;
    DuelEffectEntry *node;

    base = D_800EB0F8;
    node = base[index].entry_head_24;
    for (;;) {
        if (!(node->flags_11 & 0x80)) {
            return (DuelEffectEntry *)0;
        }
        if (node->x_0C == x && node->y_0E == y) {
            return node;
        }
        node++;
    }
}
