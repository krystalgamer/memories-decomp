#include "../types.h"
#include "script_state.h"
#include "script_op_jump_if_deck_incomplete.h"
#include "script_op_save_prompt.h"

void Script_OpJumpIfDeckIncomplete(void) {
    unsigned char *current = gScript_StreamCursor;
    unsigned int offset;
    gScript_StreamCursor = current + 2;
    offset = current[0] | (current[1] << 8);
    if (Duel_IsPlayerDeckComplete() == 0) gScript_StreamCursor = D_801A8000 + offset;
    gScript_wCommandWord = 0;
}
