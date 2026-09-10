#include "../types.h"
#include "mem_card_dialog_steps.h"
#include "mem_card_dialog_load_steps.h"
#include "mem_card_dialog_save_steps.h"
#include "mem_card_dialog_runtime.h"

/* Initialized data at 0x80090F9C: the memory card dialog step table.
 *
 * mem_card_dialog_runtime.c calls `D_80090F9C[D_8009B3DE]()`, where
 * D_8009B3DE is a u8 written by data_transfer_request.c. Five entries against
 * an unmasked byte index, so the array stays unsized for the same reason as
 * the command table above. */

void (*D_80090F9C[])(void) = {
    MemCardDialog_StepLoad,
    MemCardDialog_StepLoadUnprompted,
    MemCardDialog_StepSave,
    MemCardDialog_StepNone,
    func_8003EED0,
};
