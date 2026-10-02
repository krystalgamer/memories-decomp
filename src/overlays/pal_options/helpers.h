#ifndef PAL_OPTIONS_HELPERS_H
#define PAL_OPTIONS_HELPERS_H

#include "../../types.h"
#include "../../game/display_object.h"
#include "../../game/display_object_config.h"
#include "../../game/duel_effect_resource_setup.h"
#include "../../game/main_run_boot_sequence.h"
#include "../../game/text_constants.h"

extern u8 D_80169040[5][3];
extern s16 D_80169050;
extern u8 D_80169052;
extern s8 D_80169070;
extern s16 D_80169072;
extern DisplayObjectConfig *G32 D_80169074;
extern DisplayObjectConfig *G32 D_80169078;
extern s32 D_80169080[5][9];
extern s8 D_80169134;
extern DisplayObjectConfig *G32 D_80169138;
extern DisplayObject *G32 D_8016913C;
extern s8 D_80169140;
extern s32 D_80169144;
extern u32 D_80169148[5][9];
extern u8 D_801691FC;

void func_80168004(s32 selection);
void func_80168048(s32 selection);
s32 func_801680AC(s32 position);
void func_801686A4(s32 language);
void func_801686AC(s32 mode);
void func_80168A4C(void);
void func_80168BE8(void);
void func_80168D34(s32 language);
void func_80168D68(void);
s32 func_80168E1C(void);
s32 func_80168F70(void);
s32 func_80169030(void);

#endif
