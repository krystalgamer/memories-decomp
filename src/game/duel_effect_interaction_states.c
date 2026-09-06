#include "../types.h"
#include "input.h"

extern signed char D_8009B32C;extern unsigned short gInput_wPad1Held[],gInput_wPad1Pressed[];extern unsigned char*Dialog_OpenChoice(unsigned char*);extern void SD_SEPlayFull(int),func_8004036C(void*);
extern u16 D_8009B2AA[];
/* Separate linker names keep GCC from retaining these addresses across calls. */
extern u16 Base2_8009B2AA[];
extern u16 D_8009B2A8[];
extern u16 Base2_8009B2A8[];
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
extern u8 *D_8009B328;

extern s32 func_80036D3C(u8 *);

void func_800375A4(unsigned char*o){unsigned char f=o[0x51];if((f&0x80)==0){o[0x51]=f|0x80;D_8009B32C=10;*(void**)(o+0x30)=Dialog_OpenChoice(o);}else{if(gInput_wPad1Held[0]&PAD_BUTTON_SQUARE){D_8009B32C--;if(D_8009B32C<0)D_8009B32C=0;}else D_8009B32C=10;if(D_8009B32C!=0&&!(gInput_wPad1Pressed[0]&PAD_BUTTON_CONFIRM_MASK))return;SD_SEPlayFull(11);o[0x51]=2;func_8004036C(*(void**)(o+0x30));*(void**)(o+0x30)=0;}}

void func_8003767C(u8 *state)
{
    s32 result;

    D_8009B2AA[0] = 0;
    D_8009B2A8[0] = 0;
    result = func_80036D3C(state);
    D_8009B270[0] = result;

    if (result & 0x8000) {
        u8 **slot = (u8 **)(state + *(s8 *)(state + 0x58) * 4);
        u8 *script = *slot;
        s32 value = *script;

        *slot = script + 1;
        Base2_8009B2AA[0] = value;
        Base2_8009B2A8[0] = func_80036D3C(state);
    }

    D_8009B357 = 5;
    D_8009B27C[0] = 5;
    state[0x51] = 10;
}

void func_8003771C(u8 *object)
{
    s32 signed_value;
    s32 raw_value;

    object[0x51] = 0;
    D_8009B2A8_scalar = func_80036D3C(object);
    D_8009B2AA_scalar = func_80036D3C(object);
    D_8009B29C = func_80036D3C(object);

    signed_value = D_8009B2AA_scalar;
    raw_value = (u16)D_8009B2AA_scalar;
    if (signed_value >= 0x1000) {
        D_8009B2AA_scalar = raw_value - 0x1000;
        object[0x51] = 10;
    }

    D_8009B357 = 7;
    D_8009B27C_scalar = 7;
}

void func_800377AC(u8 *object)
{
    if (D_8009B357 == 0) {
        object[0x51] = 0;
    }
}

void func_800377C8(u8 *arg0) {
    u8 v = arg0[0x51];
    u8 *p;

    if (!(v & 0x80)) {
        arg0[0x51] = v | 0x80;
    }

    p = D_8009B328;

    if (p[0x33] != 0) {
        return;
    }

    {
        u8 w = arg0[0x51];

        if ((w & 0x40) || *(s8 *)(p + 0x30) >= 0x41) {
            arg0[0x51] = 0;

            return;
        }

        arg0[0x51] = w | 0x40;
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
