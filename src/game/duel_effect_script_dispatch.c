#include "../types.h"

typedef struct {
    u8 pad_00[0x51];
    u8 command_state;
    u8 pad_52[6];
    s8 cursor_slot;
} DuelEffectCommand;

extern void func_8003FF34(void);
extern void func_8003FF58();
extern s32 D_8009B350;

void func_80038800(DuelEffectCommand *command)
{
    register u8 **slot asm("v0");
    u8 *cursor;
    u32 raw;
    u32 saved_raw;
    register u32 opcode asm("a0");
    u32 high_bit;

    slot = (u8 **)((u8 *)command + command->cursor_slot * 4);
    cursor = *slot;
    raw = *cursor++;
    saved_raw = raw;
    opcode = saved_raw & 0x7F;
    *slot = cursor;

    if (opcode == 0) {
        func_8003FF34();
        goto shared_test;
    } else {
        func_8003FF58();
    }

shared_test:
    high_bit = saved_raw & 0x80;
    if (high_bit != 0) {
        command->command_state = 13;
        D_8009B350 = 1;
    }
}
