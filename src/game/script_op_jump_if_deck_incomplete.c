#include "../types.h"
#include "script_state.h"
#include "script_op_jump_if_deck_incomplete.h"
#include "script_op_save_prompt.h"

void Script_OpJumpIfDeckIncomplete(void) {
    unsigned char *current = D_8009B290;
    unsigned int offset;
    D_8009B290 = current + 2;
    offset = current[0] | (current[1] << 8);
    if (Duel_IsPlayerDeckComplete() == 0) D_8009B290 = D_801A8000 + offset;
    D_8009B27C = 0;
}
