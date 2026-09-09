#ifndef MEMORIES_DECOMP_TEXT_STREAM_COMMANDS_H
#define MEMORIES_DECOMP_TEXT_STREAM_COMMANDS_H

#include "../types.h"

#define TEXT_STREAM_SLOT_COUNT 22

/* Narrow view shared by handlers that advance one of the pointer slots at the
 * front of a text object. Twenty-two pointers place the signed selector at
 * the observed 0x58 offset. */
typedef struct {
    u8 *streams[TEXT_STREAM_SLOT_COUNT];
    s8 stream_index;
} TextStreamOwner;

u8 func_8003B7E0(TextStreamOwner *object);

/* Three text stream command handlers, all taking the object that owns the
 * streams.
 *
 * func_80037CE0 takes a volatile pointer. That is not decoration: the body
 * reads the halfword at 0x34 through a volatile access and pins two locals to
 * $2 and $3, so the qualifier is part of how the function is written rather
 * than a claim about the caller's storage.
 *
 * func_80037D2C and func_80037D6C both advance one of the byte streams held
 * at the front of the object, chosen by the signed byte at 0x58. */
void func_80037CE0(volatile u8 *object);
void func_80037D2C(u8 *object);
void func_80037D6C(u8 *object);

/* Writes func_80036D3C's result into the low halfword of the same word those
 * two advance: it indexes the words at the front of the object by the signed
 * byte at 0x58, and preserves the high halfword. Declared beside them because
 * it is the third reader of that selector.
 *
 * Both callers already spelled it this way -- text_handle_choice_command.c
 * calls it, and duel_effect_command_table.c takes its address for the command
 * table, which is why the declaration there is not extern. */
void Text_SetCursorOffset(u8 *object);

/* The dialog's pending completion callback, declared here because
 * func_80037CE0 is the only function ever stored in it.
 *
 * Text_HandleChoiceCommand installs it when a choice starts waiting for input,
 * func_80037CE0 clears it once the choice resolves, TextBox_BuildStep clears it
 * when a new box starts, and func_80038E1C is the only caller. Nothing else
 * writes it, so the parameter type is not a guess: it is func_80037CE0's own,
 * volatile qualifier included. */
extern void (*D_8009B340)(volatile u8 *object);

#endif
