#ifndef MEMORIES_DECOMP_DUEL_EFFECT_SCRIPT_DISPATCH_H
#define MEMORIES_DECOMP_DUEL_EFFECT_SCRIPT_DISPATCH_H

#include "../types.h"

/* The record func_80038800 walks. It covers only the three fields the handler
 * itself reaches: the command state byte at 0x51 and the signed cursor slot at
 * 0x58 that selects which of the byte streams at the front of the record the
 * opcode is read from. */
typedef struct {
    u8 pad_00[0x51];
    u8 command_state;
    u8 pad_52[6];
    s8 cursor_slot;
} DuelEffectCommand;

/* Takes the record above, not the u8 * the command table's element type
 * spells. The table casts it there. */
void func_80038800(DuelEffectCommand *command);

#endif
