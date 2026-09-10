#ifndef MEMORIES_DECOMP_MEM_CARD_DIALOG_RUNTIME_H
#define MEMORIES_DECOMP_MEM_CARD_DIALOG_RUNTIME_H

#include "../types.h"
#include "display_object.h"

/* The memory card dialog's modal runtime.
 *
 * func_8003EED0 is the fifth entry of the step table D_80090F9C
 * (mem_card_dialog_steps.c): it reads the save from each card slot in turn,
 * checks both against the staged records with SaveData_HasSameDuelistCode and
 * writes them back. MemCardDialog_Update is the frame pump that dispatches
 * that table; MemCardDialog_Poll calls it before reading the outcome. */
void func_8003EED0(void);
void MemCardDialog_Update(void);

/* Picks the first free effect channel into D_8009B3EE and opens the dialog's
 * display object into D_8009B3D8. */
void MemCardDialog_CreateObject(void);

/* One frame of the dialog box's slide. Callers load the phase at +0x60 with
 * -0x400 to open or 0x400 to close. Each call moves the phase 0x40 toward
 * zero, eases the object against (arg1, arg2) through func_80043230 and, for
 * index >= 0, moves text box `index` to the result. When the phase reaches
 * zero it clears field_6C, and that byte is the return value, so every
 * caller tests it against zero to mean "slide finished".
 *
 * The byte is returned as s32. Returning it as u8 costs nothing in the
 * definition, where the lbu already zero-extends, but makes each caller mask
 * the result before its test. */
s32 MemCardDialog_StepSlide(DisplayObject *object, s32 arg1, s32 arg2,
                            s32 index);

#endif
