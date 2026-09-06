#ifndef YUGIOH_GAME_FADE_H
#define YUGIOH_GAME_FADE_H

#include "../types.h"

#define FADE_STATE_OFFSET(type, member) ((u32)&(((type *)0)->member))

#define FADE_BAND_COUNT 30
#define FADE_BAND_HEIGHT 8
#define FADE_SCREEN_WIDTH 320
#define FADE_SCREEN_HEIGHT (FADE_BAND_COUNT * FADE_BAND_HEIGHT)
#define FADE_TRANSITION_STATE_SIZE 0x28

typedef struct {
    u8 tint_r;
    u8 tint_g;
    u8 tint_b;
    u8 pad_03;
    u8 level;
    u8 target_level;
    u8 flags;
    u8 step;
    u16 field_08;
    u8 band_levels[FADE_BAND_COUNT];
} FadeTransitionState;

typedef char FadeTransitionState_size_must_be_0x28[
    sizeof(FadeTransitionState) == FADE_TRANSITION_STATE_SIZE ? 1 : -1
];
typedef char FadeTransitionState_level_offset_must_be_0x04[
    FADE_STATE_OFFSET(FadeTransitionState, level) == 0x04 ? 1 : -1
];
typedef char FadeTransitionState_target_level_offset_must_be_0x05[
    FADE_STATE_OFFSET(FadeTransitionState, target_level) == 0x05 ? 1 : -1
];
typedef char FadeTransitionState_flags_offset_must_be_0x06[
    FADE_STATE_OFFSET(FadeTransitionState, flags) == 0x06 ? 1 : -1
];
typedef char FadeTransitionState_step_offset_must_be_0x07[
    FADE_STATE_OFFSET(FadeTransitionState, step) == 0x07 ? 1 : -1
];
typedef char FadeTransitionState_field_08_offset_must_be_0x08[
    FADE_STATE_OFFSET(FadeTransitionState, field_08) == 0x08 ? 1 : -1
];
typedef char FadeTransitionState_band_levels_offset_must_be_0x0A[
    FADE_STATE_OFFSET(FadeTransitionState, band_levels) == 0x0A ? 1 : -1
];

#undef FADE_STATE_OFFSET

extern FadeTransitionState D_800E9EC8;
extern u8 D_800E9EC8_arr[FADE_TRANSITION_STATE_SIZE];

#endif
