#ifndef PAL_OPTIONS_HELPERS_H
#define PAL_OPTIONS_HELPERS_H

#include "../../types.h"
#include "../../game/display_object.h"
#include "../../game/display_object_config.h"
#include "../../game/duel_effect_resource_setup.h"
#include "../../game/main_run_boot_sequence.h"
#include "../../game/text_constants.h"

extern s8 D_80169070;
extern DisplayObjectConfig *G32 D_80169078;
extern DisplayObject *G32 D_8016913C;
extern s8 D_80169140;

void func_80168004(s32 selection);
void func_80168048(s32 selection);
s32 func_801680AC(s32 position);
void func_801686A4(s32 language);
void func_80168D34(s32 language);
s32 func_80169030(void);

#endif
