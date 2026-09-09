#include "../types.h"
#include "duel_effect.h"
#include "duel_transition_color.h"
#include "duel_transition_step_table.h"
#include "build_deck_transition_state.h"
#include "../psyq/rand.h"

extern u32 D_8009B09C[];
extern BuildDeckTransitionState *D_8009B2FC;

s32 func_80033BE8(void)
{
    s32 intensity;
    s32 color;
    BuildDeckTransitionState *base;
    u8 *first;
    u8 *second;

    rand();

    intensity = D_8009B09C[0] & 0x3F;
    if (intensity >= 0x20) {
        intensity = 0x3F - intensity;
    }

    base = D_8009B2FC;
    color = intensity * 2 + 0x40;
    first = *(u8 **)((u8 *)base + 0x2D38);
    second = *(u8 **)((u8 *)base + 0x5A84);

    second[0xE] = color;
    second[0xD] = color;
    second[0xC] = color;
    first[0xE] = color;
    first[0xD] = color;
    first[0xC] = color;

    if (DuelEffect_UpdateState() == 0) {
        D_80090DF8[D_8009B2FC->state & 0x3F]((u8 *)D_8009B2FC);
    }

    return D_8009B2FC->state;
}
