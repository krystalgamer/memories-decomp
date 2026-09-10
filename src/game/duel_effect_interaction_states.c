#define GINPUT_PAD1_HELD_IS_AGGREGATE
#define GINPUT_PAD1_PRESSED_IS_AGGREGATE
#include "../types.h"
#include "func_80036D3C.h"
#include "input.h"
#include "sound.h"
#include "display_effect_lifecycle.h"
#include "duel_effect_interaction_states.h"
#include "dialog_choice_state.h"
#include "display_object_api.h"

extern signed char D_8009B32C;
extern u16 D_8009B2AA[];
/* Separate linker names keep GCC from retaining these addresses across calls. */
extern u16 Base2_8009B2AA[];
extern u16 D_8009B2A8[];
extern u16 Base2_8009B2A8[];
/* script_state.h declares this u16; kept here as the unsized array the
   $at store needs, since this unit cannot include that header. */
extern u16 D_8009B270[];
extern u16 D_8009B27C[];
extern u8 D_8009B357;
extern s16 D_8009B27C_scalar asm("D_8009B27C")
    __attribute__((section(".data")));
extern s16 D_8009B29C __attribute__((section(".data")));
extern s16 D_8009B2A8_scalar asm("D_8009B2A8")
    __attribute__((section(".data")));
extern s16 D_8009B2AA_scalar asm("D_8009B2AA")
    __attribute__((section(".data")));

void func_800375A4(DuelEffectChannel*o){unsigned char f=o->state_51;if((f&0x80)==0){o->state_51=f|0x80;D_8009B32C=10;o->field_30=Dialog_OpenChoice((u8*)o);}else{if(gInput_wPad1Held[0]&PAD_BUTTON_SQUARE){D_8009B32C--;if(D_8009B32C<0)D_8009B32C=0;}else D_8009B32C=10;if(D_8009B32C!=0&&!(gInput_wPad1Pressed[0]&PAD_BUTTON_CONFIRM_MASK))return;SD_SEPlayFull(11);o->state_51=2;func_8004036C(o->field_30);o->field_30=0;}}

void func_8003767C(DuelEffectChannel *state)
{
    s32 result;

    D_8009B2AA[0] = 0;
    D_8009B2A8[0] = 0;
    result = func_80036D3C(state);
    D_8009B270[0] = result;

    if (result & 0x8000) {
        u8 **slot = (u8 **)state + state->stream_58;
        u8 *script = *slot;
        s32 value = *script;

        *slot = script + 1;
        Base2_8009B2AA[0] = value;
        Base2_8009B2A8[0] = func_80036D3C(state);
    }

    D_8009B357 = 5;
    D_8009B27C[0] = 5;
    state->state_51 = 10;
}

void func_8003771C(DuelEffectChannel *object)
{
    s32 signed_value;
    s32 raw_value;

    object->state_51 = 0;
    D_8009B2A8_scalar = func_80036D3C(object);
    D_8009B2AA_scalar = func_80036D3C(object);
    D_8009B29C = func_80036D3C(object);

    signed_value = D_8009B2AA_scalar;
    raw_value = (u16)D_8009B2AA_scalar;
    if (signed_value >= 0x1000) {
        D_8009B2AA_scalar = raw_value - 0x1000;
        object->state_51 = 10;
    }

    D_8009B357 = 7;
    D_8009B27C_scalar = 7;
}

void func_800377AC(DuelEffectChannel *object)
{
    if (D_8009B357 == 0) {
        object->state_51 = 0;
    }
}

void func_800377C8(DuelEffectChannel *arg0) {
    u8 v = arg0->state_51;
    u8 *p;

    if (!(v & 0x80)) {
        arg0->state_51 = v | 0x80;
    }

    p = D_8009B328;

    if (p[0x33] != 0) {
        return;
    }

    {
        u8 w = arg0->state_51;

        if ((w & 0x40) || *(s8 *)(p + 0x30) >= 0x41) {
            arg0->state_51 = 0;

            return;
        }

        arg0->state_51 = w | 0x40;
    }

    {
        u8 *q = D_8009B328;

        q[0x33] = q[0x40];
    }

    {
        u8 *r = D_8009B328;

        *(u16 *)(r + 0x40) = 0x68;

        if (r[0x3C] != 0) {
            *(u16 *)(r + 0x40) = 0xD8;
        }
    }

    {
        u8 *s = D_8009B328;

        *(u16 *)(s + 0x42) = 0xB2;
        *(s16 *)(s + 0x44) = -0x10;
    }
}
