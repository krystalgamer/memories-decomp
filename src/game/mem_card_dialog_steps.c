#include "../types.h"
#include "mem_card_dialog_steps.h"
#include "duel_effect_state_entry.h"
#include "duel_effect_late_state.h"
#include "mem_card_dialog_runtime.h"

/* Initialized data at 0x80090F9C: the memory card dialog step table.
 *
 * mem_card_dialog_runtime.c calls `D_80090F9C[D_8009B3DE]()`, where
 * D_8009B3DE is a u8 written by data_transfer_request.c. Five entries against
 * an unmasked byte index, so the array stays unsized for the same reason as
 * the command table above. */

void (*D_80090F9C[])(void) = {
    func_8003E7D4,
    func_8003E80C,
    func_8003EE90,
    func_8003EEC8,
    func_8003EED0,
};
