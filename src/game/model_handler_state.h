#ifndef MEMORIES_DECOMP_MODEL_HANDLER_STATE_H
#define MEMORIES_DECOMP_MODEL_HANDLER_STATE_H

#include "../types.h"

/* Model-handler state and diagnostics at 0x8009AFE4-0x8009B057.
 *
 * Model_DrawSlot updates the five leading state fields. The following word
 * pairs and mutable words are consumed by the neighbouring unmatched model
 * handlers, and the fixed-size strings are their diagnostic labels and
 * formatting fragments. Address-based names remain where the exact field
 * meanings are not yet established. gModel_bBlendRateActive records whether
 * Model_DrawSlot's fade mode selected one of the four GPU blend rates
 * (mode 1-4), gModel_bPresentationTimer is the frame-step accumulator
 * Model_UpdateAnimatedBattle counts to 60 between phases, and
 * gModel_szWhyTrace is the "WHY?\n" debug-font trace the US/European
 * controllers print on a fallback path. D_8009B004 is a four-byte debug-state
 * record: two independent state bytes and a height halfword that is updated
 * unsigned but consumed signed. It is defined with
 * ModelDebug_UpdateController so MASPSX can see its small-data extent; the
 * surrounding prefix and diagnostic suffix retain their original separate
 * storage. */
extern u8 gModel_bCurrentDrawMode;
extern u8 gModel_bBlendRateActive;
extern u16 gModel_wDrawTintRedGreen;
extern u8 gModel_bDrawTintBlue;
extern u8 gModel_bPresentationTimer;
/* The mode 15 controller copies this view offset through a split absolute
 * address; its unknown-bound view changes no storage extent. */
#ifdef MODEL_HANDLER_VIEW_OFFSET_ABSOLUTE
extern u32 D_8009AFEC[];
#else
extern u32 D_8009AFEC[2];
#endif
/* The control dispatcher needs split absolute addresses for these calls.
 * Its unknown-bound view changes no storage extent. */
#ifdef MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
extern char gModel_szWhyTrace[];
#else
extern char gModel_szWhyTrace[8];
#endif
/* The unknown-bound view selects the intro controller's split absolute
 * addressing; the definition remains the same two .sdata words. */
#ifdef MODEL_HANDLER_OFFSET_ABSOLUTE
extern u32 D_8009AFFC[];
#else
extern u32 D_8009AFFC[2];
#endif
typedef union {
    u32 word;
    u8 bytes[4];
    u16 halfwords[2];
    struct {
        u8 credits_update_enabled;
        u8 credits_exit_requested;
        s16 height;
    } fields;
} ModelDebugState;

typedef char ModelDebugState_size_must_be_4[
    sizeof(ModelDebugState) == 4 ? 1 : -1
];
typedef char ModelDebugState_height_offset_must_be_2[
    (u32)&((ModelDebugState *)0)->fields.height == 2 ? 1 : -1
];

extern ModelDebugState D_8009B004;
typedef union {
    u32 word;
    u8 display_enabled;
} ModelDebugDisplayState;

typedef char ModelDebugDisplayState_size_must_be_4[
    sizeof(ModelDebugDisplayState) == 4 ? 1 : -1
];

extern ModelDebugDisplayState gModelDebug_Display;
#ifdef MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
/* The debug controller uses absolute string addresses. Their definitions
 * retain eight bytes each (four for B02C); no larger backing is implied. */
extern char gModelDebug_szNormalColor[];
extern char gModelDebug_szSelectedColor[];
extern char gModelDebug_szDigitFormat[];
extern char gModelDebug_szVersusSeparator[];
extern char gModelDebug_szLineBreak[];
#else
extern char gModelDebug_szNormalColor[8];
extern char gModelDebug_szSelectedColor[8];
extern char gModelDebug_szDigitFormat[8];
extern char gModelDebug_szVersusSeparator[8];
extern char gModelDebug_szLineBreak[4];
#endif
#ifdef MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
extern char gModel_szBackTrace[];
extern char gModel_szStoneTrace[];
extern char gModel_szHitTrace[];
extern char gModel_szOuchTrace[];
extern char gModel_szTermTrace[];
#else
extern char gModel_szBackTrace[8];
extern char gModel_szStoneTrace[8];
extern char gModel_szHitTrace[8];
extern char gModel_szOuchTrace[8];
extern char gModel_szTermTrace[8];
#endif

extern char gModel_szContinueTrace[];

/* Original sequence-handler assertion format and source-file label. */
extern char gModelAnim_szAssertFormat[];
extern char gModelAnim_szSourceFileName[];

#endif
