#ifndef GAME_DIALOG_CHOICE_CONTROL_H
#define GAME_DIALOG_CHOICE_CONTROL_H

#include "../types.h"

#ifdef D_8009B34C_IN_DATA
extern u8 gDialog_bChoiceControl __attribute__((section(".data")));
#else
extern u8 gDialog_bChoiceControl;
#endif

#endif
