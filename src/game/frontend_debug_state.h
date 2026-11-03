#ifndef MEMORIES_DECOMP_FRONTEND_DEBUG_STATE_H
#define MEMORIES_DECOMP_FRONTEND_DEBUG_STATE_H

#include "../types.h"
#include "display_object.h"

enum { FRONTEND_DEBUG_ROW_COUNT = 3 };

#define FRONTEND_STEP_INDEX_MASK 0x1F
#define FRONTEND_STEP_FLAG_ASYNC_PENDING 0x40
#define FRONTEND_STEP_FLAG_ENTERED 0x80

typedef s8 FrontendDebugColumns[FRONTEND_DEBUG_ROW_COUNT];
typedef u8 FrontendDebugDigitCounts[FRONTEND_DEBUG_ROW_COUNT];
typedef struct {
    u16 row[FRONTEND_DEBUG_ROW_COUNT];
} FrontendDebugValues;
typedef u16 FrontendDebugSoundValues[2];
typedef union {
    u8 *bytes;
    u16 *words;
} FrontendDebugRowPointer;

/* Edits the active frontend debug value and redraws its digit caret. */
s32 DebugMenu_UpdateValueEditor(void);

/* A six-byte extern changes assembler -G4 relocations in the editor's split
 * profile. The bounded value view is taken from this scalar label instead. */
extern u16 gDebug_nSceneOrSoundID;

#ifdef FRONTEND_DEBUG_SOUND_ROW_VIEW
extern FrontendDebugSoundValues gDebug_nLastSoundID;
#else
extern u16 gDebug_nLastSoundID;
#endif

extern u8 D_8009B2B2;
extern u8 gDebugMenu_bCampaignEditField;
extern u16 gDebug_nEditedBgmId;
extern u16 gDebug_nEditedCdSoundId;
extern u16 gDebug_nLastBgmId;
extern s8 gDebug_bEditorRow;
extern u16 D_8009B2DE;
extern DisplayObject *gDebugMenu_pCursorQuad;
extern u8 gDebugMenu_bLastMapId;
extern s8 gDebug_bEditorDigit;
extern u8 gDebugMenu_bStep;
extern u8 gDebugMenu_bPage;
extern u8 gDebug_szEditorCaretLine[];

typedef char FrontendDebugColumnsSize[
    sizeof(FrontendDebugColumns) == 3 ? 1 : -1];
typedef char FrontendDebugDigitCountsSize[
    sizeof(FrontendDebugDigitCounts) == 3 ? 1 : -1];
typedef char FrontendDebugValuesSize[
    sizeof(FrontendDebugValues) == 6 ? 1 : -1];
typedef char FrontendDebugValueRow1Offset[
    (u32)&((FrontendDebugValues *)0)->row[1] == 2 ? 1 : -1];
typedef char FrontendDebugValueRow2Offset[
    (u32)&((FrontendDebugValues *)0)->row[2] == 4 ? 1 : -1];
typedef char FrontendDebugSoundValuesSize[
    sizeof(FrontendDebugSoundValues) == 4 ? 1 : -1];

#endif
