#ifndef MEMORIES_DECOMP_MODEL_HANDLER_STATE_H
#define MEMORIES_DECOMP_MODEL_HANDLER_STATE_H

#include "../types.h"

/* Model-handler state at 0x8009AFE4-0x8009B057.
 *
 * Model_DrawSlot updates the five leading state fields. The following word
 * pairs and mutable words are consumed by the neighbouring model handlers,
 * and the string literals model.c prints occupy the rest of the range.
 * Address-based names remain where the exact field meanings are not yet
 * established. gModel_bBlendRateActive records whether Model_DrawSlot's fade
 * mode selected one of the four GPU blend rates (mode 1-4), and
 * gModel_bPresentationTimer is the frame-step accumulator
 * Model_UpdateAnimatedBattle counts to 60 between phases. D_8009B004 is a
 * four-byte debug-state record: two independent state bytes and a height
 * halfword that is updated unsigned but consumed signed. It is defined with
 * ModelDebug_UpdateController so MASPSX can see its small-data extent. */
extern u8 gModel_bCurrentDrawMode;
extern u8 gModel_bBlendRateActive;
extern u16 gModel_wDrawTintRedGreen;
extern u8 gModel_bDrawTintBlue;
extern u8 gModel_bPresentationTimer;
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
extern const char gModel_szContinueTrace[];

#endif
