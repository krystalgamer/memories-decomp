#ifndef MEMORIES_DECOMP_OVERLAYS_PASSWORD_MODULE_STATE_H
#define MEMORIES_DECOMP_OVERLAYS_PASSWORD_MODULE_STATE_H

#include "../../types.h"
#include "../../ygo_types.h"

extern PasswordModuleState gPassword_ModuleState;

extern u8 D_8016D400;
extern s8 D_8016D401;
extern u8 D_8016D402;
extern u8 D_8016D403;
extern SelectionFrame *D_8016D404;
extern u8 D_8016D408;
extern u8 gPassword_abDigits[];
extern u8 *D_8016D418;
extern u8 D_8016D41C;
extern PasswordCursorView *gPassword_pDigitCursorWidget;
extern u16 D_8016D424;
extern u8 D_8016D426;
extern s32 gPassword_nDigitIndex;
extern s8 D_8016D42C;
extern struct DuelEffectResourceRecord *D_8016D430;
extern s16 D_8016D434;
extern s16 D_8016D436;
extern u32 D_8016D438;
extern u8 *D_8016D43C;
extern struct DisplayObject *D_8016D440[];
extern u8 D_8016D4D0;
extern u16 D_8016D4D2;
extern u16 D_8016D4D4;
extern PasswordCardPreviewView *D_8016D4D8;
extern u16 D_8016D4DC;

#endif
