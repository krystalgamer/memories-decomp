#ifndef MEMORIES_DECOMP_FUNC_8003B378_H
#define MEMORIES_DECOMP_FUNC_8003B378_H

#include "../types.h"

/* Ticks one menu record's display effect. It reads the record as bytes --
 * the flag byte at 0x32, the object pointer at 0x04 and the delay halfword
 * at 0x3E -- which is why the parameter is u8 * rather than MenuRecord *.
 *
 * Both consumers hold the record under a different view and cross to the byte
 * view at the call: display_effect_process_menu_records.c already writes
 * (u8 *)record, and func_80031354.c passes D_800EB010, which menu_record.h
 * declares MenuRecord []. */
void func_8003B378(u8 *p, s32 n);

#endif
