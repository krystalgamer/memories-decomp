#ifndef YUGIOH_GAME_MODEL_OBJECT_HANDLER_ENTRIES_H
#define YUGIOH_GAME_MODEL_OBJECT_HANDLER_ENTRIES_H

#include "../types.h"

/* The handler entry points func_800608B8 dispatches to, declared once for the
 * two files that hand out their addresses.
 *
 * All sixteen are hand-written assembly with no defining C translation unit,
 * and both consumers only ever take their addresses -- `return (s32)func_...`
 * -- so nothing in the tree ever checked one declaration against the other.
 * They had drifted: func_800608B8.c spelled them `int X()` while
 * model_handler_registry.c spelled eight of them `void X(void)`.
 *
 * The retail image settles it. Every one of the sixteen reads its first
 * argument through $a0 -- func_80067354 begins `lw $a3, 0x0($a0)` and saves
 * $s0..$s3 into 0x28($a0)..0x34($a0) -- and every one leaves a value in $v0
 * immediately before `jr $ra`; func_80067354 ends
 * `lw $v0, 0x0($a0)` / `addiu $v0, $v0, 0x8`. So `void X(void)` was wrong in
 * both return type and arity, and the unprototyped form kept here is the one
 * the image does not contradict.
 *
 * What this does NOT claim: the parameter and return types are deliberately
 * left unspecified. The evidence shows that an argument is taken and a value
 * returned, not what either is. $a0 is dereferenced as a pointer and the
 * result is derived from it, but nothing here establishes the pointee's
 * layout, so writing a fuller prototype would assert more than is known. */
int func_80067354();
int func_8006759C();
int func_80067858();
int func_80067ABC();
int func_80067D94();
int func_80067FD0();
int func_8006825C();
int func_800684B4();
int func_8006875C();
int func_80068A00();
int func_80068D18();
int func_80068FD8();
int func_8006930C();
int func_800695A4();
int func_8006988C();
int func_80069B40();

#endif
