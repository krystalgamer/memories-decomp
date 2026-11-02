#ifndef PAL_OPTIONS_HELPERS_H
#define PAL_OPTIONS_HELPERS_H

#include "../../types.h"
#include "../../game/display_object.h"
#include "../../game/display_object_config.h"
#include "../../game/duel_effect_resource_setup.h"
#include "../../game/main_run_boot_sequence.h"
#include "../../game/text_constants.h"

extern u8 gPalOptions_aStripTextures[5][3];
extern s16 gPalOptions_wStripScroll;
extern u8 gPalOptions_bMode;
extern s8 gPalOptions_bSoundOutputType;
extern s16 gPalOptions_wStripScrollTarget;
extern DisplayObjectConfig *G32 gPalOptions_pLanguageSprite;
extern DisplayObjectConfig *G32 gPalOptions_pSoundTypeSprite;
extern s32 gPalOptions_aWaveOffsets[5][9];
extern s8 gPalOptions_bSelectedRow;
extern DisplayObjectConfig *G32 gPalOptions_pLabelSprite;
extern DisplayObject *G32 gPalOptions_pCursor;
extern s8 gPalOptions_bLanguage;
extern s32 gPalOptions_dwWavePhase;
extern u32 gPalOptions_aWaveColors[5][9];
extern u8 gPalOptions_bState;

void PalOptions_SetTextColorSlots(s32 selection);
void PalOptions_PositionCursor(s32 selection);
s32 PalOptions_EaseStripY(s32 position);
void func_801686A4(s32 language);
void Options_Init(s32 mode);
void PalOptions_HandleInput(void);
void PalOptions_UpdateWaveTables(void);
void PalOptions_RequestLanguage(s32 language);
void PalOptions_StoreLanguageImage(void);
s32 Options_Update(void);
s32 PalOptions_StepStripScroll(void);
s32 PalOptions_GetLanguage(void);

#endif
