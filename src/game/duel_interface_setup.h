#ifndef MEMORIES_DECOMP_DUEL_INTERFACE_SETUP_H
#define MEMORIES_DECOMP_DUEL_INTERFACE_SETUP_H

#include "../types.h"
#include "frontend_debug_state.h"

void DebugMenu_Init(void);
void DebugMenu_OpenValueEditor(
    u8 *data,
    s32 caret_column_0,
    s32 caret_column_1,
    s32 caret_column_2,
    s32 leading_line_count,
    s32 digit_count,
    s32 row_count
);

#endif
