#ifndef GAME_DIALOG_CHOICE_CONTROL_H
#define GAME_DIALOG_CHOICE_CONTROL_H

#include "../types.h"

#define DIALOG_CHOICE_CONTROL_MASK 0xF0
#define DIALOG_CHOICE_CONTROL_LAYOUT_MASK 0x30
#define DIALOG_CHOICE_CONTROL_RETAIN_HIGHLIGHT 0x40

/* Layout bits remain a raw masked offset in geometry. A nonzero value also
 * makes the completed text-choice layout publish a count of two. */
#ifdef D_8009B34C_IN_DATA
extern u8 gDialog_bChoiceControl __attribute__((section(".data")));
#else
extern u8 gDialog_bChoiceControl;
#endif

#endif
