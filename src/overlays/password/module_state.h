#ifndef MEMORIES_DECOMP_OVERLAYS_PASSWORD_MODULE_STATE_H
#define MEMORIES_DECOMP_OVERLAYS_PASSWORD_MODULE_STATE_H

#include "../../types.h"
#include "../../ygo_types.h"
#ifdef VERSION_JAPAN
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#endif

extern struct DisplayObject *gNameEntry_apKeySprites[3];
extern u8 gNameEntry_bStateFlags;
extern s8 gNameEntry_bKeyboardColumn;
extern u8 gNameEntry_bKeyboardRow;
extern u8 gNameEntry_bGlyphVramSlot;
extern struct DisplayObject *gNameEntry_pSelectionFrame;
extern u8 gNameEntry_bGlyphSequence;
extern u8 gPassword_abDigits[];
extern u8 *gNameEntry_pNameBuffer;
extern u8 D_8016D41C;
extern struct DisplayObject *gPassword_pDigitCursorWidget;
extern u16 gPassword_wShopState;
extern u8 gNameEntry_bSavedRow;
extern s32 gPassword_nDigitIndex;
extern s8 gNameEntry_bCaretPos;
extern struct DuelEffectResourceRecord *gPassword_pCardCache;
extern s16 gNameEntry_wCursorTargetX;
extern s16 gNameEntry_wCursorTargetY;
extern u32 gPassword_dwPriceRemaining;
extern struct DisplayObject *gNameEntry_pCaret;
extern struct DisplayObject *gPassword_apDigitCursorDecorations[];
#ifdef VERSION_JAPAN
extern RECT gNameEntry_aGlyphVramRects[16];
#endif
extern u8 gNameEntry_bCharacterSet;
extern u16 gNameEntry_wPendingDialogId;
extern u16 gNameEntry_wSelectionFrameFlags;
extern struct DisplayObject *gPassword_pCardPreview;
extern u16 gPassword_wCardId;

#endif
