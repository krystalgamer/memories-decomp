#include "../types.h"
#include "view_state.h"

extern void func_800857C0(int);
extern void func_8001352C(void);

void func_80017130(void)
{
    ViewState *s = &D_800F2848;
    /* This second base pointer is load-bearing, not leftover noise. Written
       as s->field_1C, s->field_20 and s->field_24 the three stores share the
       first base with plain displacements and the function comes out one
       instruction short of retail; the original clearly had a cursor here. */
    s32 *words = (s32 *)((u8 *)s + 0x10);

    s->field_00 = 0x258;
    s->angle = 0x400;
    s->field_04 = 0x100;
    s->field_0C = 0;
    s->field_28 = 0;
    s->field_2C = 0;
    s->projection = 0x12C;
    func_800857C0(0x12C);
    words[3] = 0;
    s->field_06 = 0;
    words[4] = 0;
    s->field_08 = 0;
    words[5] = 0;
    s->field_0A = 0;
    func_8001352C();
}
