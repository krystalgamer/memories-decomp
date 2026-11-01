#ifndef MEMORIES_DECOMP_MEM_CARD_DIALOG_H
#define MEMORIES_DECOMP_MEM_CARD_DIALOG_H

#include "../types.h"
#include "display_object.h"

/* The first four entries of the memory-card dialog step table gMemCardDialog_apfnSteps.
 * The load and save callbacks latch DUEL_EFFECT_STATE_FLAG_INITIALIZED, seed
 * gMemCardDialog_OperationState, and then pump the corresponding state machine. Step 1 skips the
 * load confirmation prompt. NA step 3 is empty; the Japanese and European
 * builds use MemCardDialog_StepAlternateSave instead. */
extern void MemCardDialog_UpdateLoad(void);
void MemCardDialog_StepLoad(void);
void MemCardDialog_StepLoadUnprompted(void);
void MemCardDialog_UpdateSave(void);
void MemCardDialog_StepSave(void);
void MemCardDialog_StepNone(void);
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
void MemCardDialog_StepAlternateSave(void);
#endif

/* The memory card dialog's modal runtime.
 *
 * MemCardDialog_UpdateTradeSave is the fifth entry of the step table
 * gMemCardDialog_apfnSteps (mem_card_dialog_steps.c), selected only by
 * SaveData_RequestTradeWrite: it reads the save from each card slot in turn,
 * checks both against the staged records with SaveData_HasSameDuelistCode
 * and writes them back in alternating 0x80-byte chunks. MemCardDialog_Update
 * is the frame pump that dispatches that table; the request API at the end of
 * the same translation unit calls it before reading the outcome. */
void MemCardDialog_UpdateTradeSave(void);
extern void MemCardDialog_Update(void);

#ifdef VERSION_JAPAN
/* The Japanese dialog brackets the card session with these SDK helpers. */
void func_8008D680(void);
void func_8008D6F0(void);
#endif

/* Selects the first inactive channel, or retains zero if all are active,
 * then creates the display object. Selection is not channel reservation. */
void MemCardDialog_CreateObject(void);

/* One frame of the dialog box's slide. Callers load the phase at +0x60 with
 * -0x400 to open or 0x400 to close. Each call moves the phase 0x40 toward
 * zero, eases the object against (arg1, arg2) through Widget_SlideSine and, for
 * index >= 0, moves text box `index` to the result. When the phase reaches
 * zero it clears field_6C, and that byte is the return value, so every
 * caller tests it against zero to mean "slide finished".
 * Japanese text-box channels have a 0x60-byte stride instead of 0x64.
 *
 * The byte is returned as s32. Returning it as u8 costs nothing in the
 * definition, where the lbu already zero-extends, but makes each caller mask
 * the result before its test. */
s32 MemCardDialog_StepSlide(DisplayObject *object, s32 arg1, s32 arg2,
                            s32 index);

#endif
