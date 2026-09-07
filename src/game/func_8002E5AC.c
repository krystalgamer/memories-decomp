#include "../types.h"

extern u8 *D_8009B290;
extern u16 D_8009B27C;
/* Defined rather than declared: the assembler only resolves a small
   global gp-relative when the translation unit defines it, which is what
   makes the store below a single %gp_rel instruction whose load-delay
   slot carries the retail nop. c_symbols.ld overrides this common symbol,
   so no storage is allocated here. */
u16 D_8009B28C;
extern u16 D_8009B2A4;
extern u8 D_800EB0F8[];

extern s32 func_8002E3B4(void);
extern void func_8003B6AC(s32, s32);
extern u8 *TextBox_Create(s32, s32, s32, s32, s32, s32);
extern void DuelEffect_MarkObjectIfActive(void *);
extern void TextBox_Destroy(void *);

void func_8002E5AC(void)
{
    u8 *script;
    u8 *box;
    s32 value;
    u16 flags;
    u16 boxflags;

    if (func_8002E3B4() == 0) {
        script = D_8009B290;
        D_8009B290 = script + 2;
        value = script[0] | (script[1] << 8);
        D_8009B2A4 |= 0x4000;
        func_8003B6AC(0, 2);
        box = TextBox_Create(0, value & 0xFFF, 0x10, 0xB0, 0x120, 0x30);
        DuelEffect_MarkObjectIfActive(box);
        *(u16 *)(box + 0x34) |= 8;
        if ((value & 0x8000) != 0) {
            flags = D_8009B27C;
            boxflags = *(volatile u16 *)(box + 0x34);
            D_8009B27C = flags | 0x4000;
            *(u16 *)(box + 0x34) = boxflags & 0xFFF7;
        }
        D_8009B28C = D_8009B27C;
    } else {
        if ((D_8009B2A4 & 0x4000) == 0) {
            if ((D_8009B27C & 0x4000) == 0) {
                TextBox_Destroy(D_800EB0F8);
            }
            D_8009B28C = 0;
            D_8009B27C = 0;
        }
    }
}
