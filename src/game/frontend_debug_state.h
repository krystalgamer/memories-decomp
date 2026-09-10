#ifndef MEMORIES_DECOMP_FRONTEND_DEBUG_STATE_H
#define MEMORIES_DECOMP_FRONTEND_DEBUG_STATE_H

#include "../types.h"
#include "display_object.h"

enum { FRONTEND_DEBUG_ROW_COUNT = 3 };

typedef s8 FrontendDebugColumns[FRONTEND_DEBUG_ROW_COUNT];
typedef u8 FrontendDebugDigitCounts[FRONTEND_DEBUG_ROW_COUNT];
typedef struct {
    u16 row[FRONTEND_DEBUG_ROW_COUNT];
} FrontendDebugValues;
typedef u16 FrontendDebugSoundValues[2];

/* The editor walks three adjacent rows; scalar producers retain their own
 * linker labels and small-data accesses. See notes/frontend-debug-state.md. */
#ifdef FRONTEND_DEBUG_ROW_VIEWS
extern FrontendDebugColumns D_8009B2B4;
extern FrontendDebugDigitCounts D_8009B2C0;
#else
extern u8 D_8009B2B4;
extern u8 D_8009B2C0;
#endif
/* A six-byte extern changes assembler -G4 relocations in the editor's split
 * profile. The bounded value view is taken from this scalar label instead. */
extern u16 gDebug_nSceneOrSoundID;

#ifdef FRONTEND_DEBUG_SOUND_ROW_VIEW
extern FrontendDebugSoundValues gDebug_nLastSoundID;
#else
extern u16 gDebug_nLastSoundID;
#endif

extern u8 D_8009B2B2;
extern u8 D_8009B2B5;
extern u8 D_8009B2B6;
extern u8 D_8009B2B8;
extern u8 D_8009B2C1;
extern u8 D_8009B2C2;
extern u16 D_8009B2CA;
extern u16 D_8009B2CC;
extern u16 D_8009B2DA;
extern u8 D_8009B2DC;
extern u16 D_8009B2DE;
extern u8 D_8009B2E0;
extern DisplayObject *D_8009B2E4;
extern u8 D_8009B2E8;
extern u8 D_8009B2E9;
extern u8 D_8009B2EA;
extern u8 D_8009B2EB;
extern u8 *D_8009B2EC;
extern u8 D_8009B2F0;
extern u8 D_800EAED8[];

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
