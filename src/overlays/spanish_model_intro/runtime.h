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
} ModelIntroCue;

typedef struct {
    s16 delay;
    u8 phase;
    u8 field_03;
    u8 cue;
    u8 active;
} ModelIntroChannel;

extern ModelIntroCue D_801805D0[32];
extern u8 D_80180650;
extern ModelIntroChannel D_80180654[4];
extern DisplayObject *D_8018066C;
extern u32 D_80180670;

void func_80180004(s32 index);
s32 func_8018019C(void);
void func_80180420(void);
s32 func_801804A0(void);
void func_801804B0(DisplayObject *object, GsOT *ot);

#endif
