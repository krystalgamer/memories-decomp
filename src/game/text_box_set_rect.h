#ifndef MEMORIES_DECOMP_TEXT_BOX_SET_RECT_H
#define MEMORIES_DECOMP_TEXT_BOX_SET_RECT_H

#include "../types.h"

typedef struct {
    u8 pad_00[0x3C];
    s16 field_3C;
    s16 field_3E;
    s16 field_40;
    s16 field_42;
    u8 pad_44[0x1C];
} JapaneseTextBoxRectChannel;

typedef char JapaneseTextBoxRectChannel_size_must_be_0x60[
    sizeof(JapaneseTextBoxRectChannel) == 0x60 ? 1 : -1
];

void TextBox_SetRect(s32, s32, s32, s32, s32);

#endif
