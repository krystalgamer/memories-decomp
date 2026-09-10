#ifndef YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_ENTRIES_H
#define YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_ENTRIES_H

#include "../types.h"

/* The primitive handler entry points, declared once for the two files that
 * hand out their addresses.
 *
 * All are hand-written assembly with no defining C translation unit, so there
 * is no per-TU header for them to live in. Both model_primitive_handler.c and
 * model_handler_registry.c were declaring the same set locally, and both only
 * ever take their addresses -- never call them -- so nothing in the tree
 * checked either copy against the other. That is the shape that lets two
 * declarations of one function drift apart silently.
 *
 * These sixteen overlapping names already agreed exactly, void (void) on both
 * sides, so this decides only where the declaration lives.
 *
 * This is deliberately a separate header from model_primitive_handler.h
 * rather than an addition to it. That header also declares func_800603DC as
 * void *(u32), while model_handler_registry.c declares the same function
 * extern s32 (*func_800603DC())() and uses it sixty-seven times. Those two
 * spellings collide, so the registry cannot include model_primitive_handler.h
 * at all; keeping the entry points apart is what lets both files share them.
 *
 * The order is retail's own arm order in memory. */
void func_800612C0(void);
void func_8006151C(void);
void func_800617E0(void);
void func_80061A84(void);
void func_80061DDC(void);
void func_80062058(void);
void func_8006233C(void);
void func_80062600(void);
void func_80062978(void);
void func_80062BC0(void);
void func_80062E70(void);
void func_80063100(void);
void func_80063444(void);
void func_800636AC(void);
void func_8006397C(void);
void func_80063C2C(void);
void func_80063F90(void);
void func_80064248(void);
void func_80064568(void);
void func_80064868(void);
void func_80064C1C(void);
void func_80064EF4(void);
void func_80065234(void);
void func_80065554(void);
void func_80065928(void);
void func_80065BCC(void);
void func_80065ED8(void);
void func_800661C4(void);
void func_80066564(void);
void func_80066828(void);
void func_80066B54(void);
void func_80066E60(void);

#endif
