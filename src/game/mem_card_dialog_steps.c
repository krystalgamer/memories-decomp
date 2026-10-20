#include "../types.h"
#include "mem_card_dialog_steps.h"
#include "mem_card_dialog.h"

/* Initialized data at 0x80090F9C: the memory card dialog step table.
 *
 * mem_card_dialog.c calls `gMemCardDialog_apfnSteps[gMemCardDialog_StepIndex]()`, where
 * gMemCardDialog_StepIndex is a u8 written by that runtime's request API. Five entries against
 * an unmasked byte index, so the array stays unsized for the same reason as
 * the command table above. */

void (*gMemCardDialog_apfnSteps[])(void) = {
    MemCardDialog_StepLoad,
    MemCardDialog_StepLoadUnprompted,
    MemCardDialog_StepSave,
    MemCardDialog_StepNone,
    MemCardDialog_UpdateTradeSave,
};
