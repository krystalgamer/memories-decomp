#ifndef MEMORIES_DECOMP_OVERLAYS_PASSWORD_MODULE_STATE_H
#define MEMORIES_DECOMP_OVERLAYS_PASSWORD_MODULE_STATE_H

#include "../../types.h"
#include "../../ygo_types.h"

extern PasswordModuleState gPassword_ModuleState;

extern struct DisplayObject *G32 D_8016D290[3];
extern u8 D_8016D400;
extern s8 gNameEntry_bKeyboardColumn;
extern u8 gNameEntry_bKeyboardRow;
extern u8 D_8016D403;
extern SelectionFrame *G32 gNameEntry_pSelectionFrame;
extern u8 gNameEntry_bGlyphSequence;
extern u8 gPassword_abDigits[];
extern u8 *G32 gNameEntry_pNameBuffer;
extern u8 D_8016D41C;
extern PasswordCursorView *G32 gPassword_pDigitCursorWidget;
extern u16 gPassword_wShopState;
extern u8 gNameEntry_bSavedRow;
extern s32 gPassword_nDigitIndex;
extern s8 gNameEntry_bCaretPos;
extern struct DuelEffectResourceRecord *G32 gPassword_pCardCache;
extern s16 gNameEntry_wCursorTargetX;
extern s16 gNameEntry_wCursorTargetY;
extern u32 gPassword_dwPriceRemaining;
extern u8 *G32 gNameEntry_pCaret;
extern struct DisplayObject *G32 gPassword_apDigitCursorDecorations[];
extern u8 D_8016D4D0;
extern u16 gNameEntry_wPendingDialogId;
extern u16 D_8016D4D4;
extern PasswordCardPreviewView *G32 gPassword_pCardPreview;
extern u16 gPassword_wCardId;

#endif
