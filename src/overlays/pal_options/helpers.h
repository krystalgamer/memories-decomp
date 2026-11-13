#ifndef PAL_OPTIONS_HELPERS_H
#define PAL_OPTIONS_HELPERS_H

#include "../../types.h"
#include "../../game/display_object.h"
#include "../../game/display_object_system.h"
#include "../../game/card_preview.h"
#include "../../game/main_boot_frontend.h"
#include "../../game/text_constants.h"

extern u8 gPalOptions_aStripTextures[5][3];
extern s16 gPalOptions_wStripScroll;
extern u8 gPalOptions_bMode;
extern s8 gPalOptions_bSoundOutputType;
extern s16 gPalOptions_wStripScrollTarget;
extern DisplayObject *gPalOptions_pLanguageSprite;
extern DisplayObject *gPalOptions_pSoundTypeSprite;
extern s32 gPalOptions_aWaveOffsets[5][9];
extern s8 gPalOptions_bSelectedRow;
extern DisplayObject *gPalOptions_pLabelSprite;
extern DisplayObject *gPalOptions_pCursor;
extern s8 gPalOptions_bLanguage;
extern s32 gPalOptions_dwWavePhase;
extern u32 gPalOptions_aWaveColors[5][9];
extern u8 gPalOptions_bState;
/* Storage of the unreferenced unoptimized suffix copies. */
extern s16 gPalOptions_wStripScrollCopy;
extern s16 gPalOptions_wStripScrollTargetCopy;
extern s8 gPalOptions_bLanguageCopy;

void PalOptions_SetTextColorSlots(s32 selection);
void PalOptions_PositionCursor(s32 selection);
s32 PalOptions_EaseStripY(s32 position);
void PalOptions_NoOpLanguageHook(s32 language);
void Options_Init(s32 mode);
void PalOptions_HandleInput(void);
void PalOptions_UpdateWaveTables(void);
void PalOptions_RequestLanguage(s32 language);
void PalOptions_StoreLanguageImage(void);
s32 Options_Update(void);
s32 PalOptions_StepStripScroll(void);
s32 PalOptions_GetLanguage(void);
s32 PalOptions_StepStripScrollCopy(void);
s32 PalOptions_GetLanguageCopy(void);

#endif
