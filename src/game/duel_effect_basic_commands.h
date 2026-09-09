#ifndef MEMORIES_DECOMP_DUEL_EFFECT_BASIC_COMMANDS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_BASIC_COMMANDS_H

#include "../types.h"

/* Three D_80090EAC entries. func_80038898 reads one byte from the object's
 * current stream into D_8009B363 and puts the scene mode at 5.
 *
 * func_80038888 and func_80038890 take NO argument at all -- they are defined
 * `void (void)` and their bodies are empty -- while the table's entry type is
 * `void (*)(u8 *)`. The table casts them, which is the same shape #3073
 * settled on for func_80038070: the real prototype lives here and the cast is
 * at the one place that needs it. They are also two distinct addresses rather
 * than one entry repeated. */
void func_80038888(void);
void func_80038890(void);
void func_80038898(u8 *object);

#endif
