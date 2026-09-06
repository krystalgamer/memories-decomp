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
