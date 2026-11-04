#ifndef MEMORIES_DECOMP_SPANISH_MODEL_INTRO_RUNTIME_H
#define MEMORIES_DECOMP_SPANISH_MODEL_INTRO_RUNTIME_H

#include "../../types.h"
#include "../../game/duel_effect.h"
#include "../../game/duel_effect_entry_control.h"
#include "../../game/duel_effect_process_entries.h"
#include "../../game/text_box_lifecycle.h"
#include "../../game/text_box_runtime.h"
#include "../../game/display_object.h"
#include "../../game/display_object_core.h"
#include "../../game/display_object_helpers.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_frame.h"

typedef struct {
    u8 text;
    u8 secondary;
    u8 clear_secondary;
    u8 flags;
} CreditsCue;

typedef struct {
    s16 delay;
    u8 phase;
    u8 field_03;
    u8 cue;
    u8 active;
} CreditsChannel;

extern CreditsCue gCredits_aCues[32];
extern u8 gCredits_bFinished;
extern CreditsChannel gCredits_aChannels[4];
extern DisplayObject *gCredits_pGridObject;
extern u32 gCredits_dwFrameCounter;

void Credits_StartLineGroup(s32 index);
s32 Credits_UpdateLines(void);
void Credits_InitPresentation(void);
s32 Credits_IsFinished(void);
void Credits_DrawGrid(DisplayObject *object, GsOT *ot);

#endif
