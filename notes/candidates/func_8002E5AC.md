## `func_8002E5AC` at 0x8002E5AC

`gcc_2_8_1_g8_split`, 68 instructions against a target of 67, opcode
distance 3.

The script opcode that opens a dialogue box. When `func_8002E3B4` reports
idle it reads a little-endian `u16` out of the `D_8009B290` script stream,
advances the stream by two, sets bit `0x4000` of `D_8009B2A4`, calls
`func_8003B6AC(0)`, and creates a text box with the low 12 bits as its id.
The new object gets bit `8` at `+0x34`; when the script word has bit `0x8000`
the box is instead marked in `D_8009B27C` and that bit is cleared again. The
other path is the teardown: it returns while `D_8009B2A4` still has `0x4000`,
otherwise destroys the box unless `D_8009B27C` has `0x4000`, and clears both
`D_8009B28C` and `D_8009B27C`.

The stream read reuses the idiom from the matched neighbour
`func_8002E470.c`, which shares the `func_8002E3B4` guard and the same
`D_8009B290` cursor: take the cursor into a local, form the next pointer as
its own local, publish that, then index the original. Every global except
`D_8009B28C` already had a declaration in a sibling.

What remains is one instruction and three opcodes: one `addu` missing against
two extra `nop`. Note that `nop` is `sll $zero, $zero, 0`, so an opcode census
reports these as extra `sll` -- they are padding, not arithmetic.

Both differences are the same fact. The target fills the load-delay slot after
`lw $v1, D_8009B290` by splitting the stream advance into
`addiu $a1, $zero, 0x2` and `addu $v0, $v1, $a1`, spending an extra
instruction to keep the slot busy; this build emits `addiu $v0, $v1, 2` and
pads with `nop`. Giving the 2 its own local does not reproduce the split, and
neither does advancing the cursor inline.

Crossed without improving on distance 3: the stream advance as a named local,
inline, and through a separate next pointer; the `+0x34` read-modify-write
with and without a temporary for the second read; and three profiles. Reading
`+0x34` into a temporary reaches 66 instructions rather than 68, at the same
distance.

```c
#include "../../src/types.h"

extern u8 *D_8009B290;
extern u16 D_8009B2A4;
extern u16 D_8009B27C;
extern u16 D_8009B28C;
extern u8 D_800EB0F8[];

extern s32 func_8002E3B4(void);
extern void func_8003B6AC(s32);
extern void *TextBox_Create(s32, s32, s32, s32, s32, s32);
extern void TextBox_Destroy(void *);
extern void DuelEffect_MarkObjectIfActive(void *);

void func_8002E5AC(void)
{
    u8 *script;
    u8 *next;
    s32 value;
    u8 *box;

    if (func_8002E3B4() == 0) {
        script = D_8009B290;
        next = script + 2;
        D_8009B290 = next;
        value = script[0] | (script[1] << 8);
        D_8009B2A4 |= 0x4000;
        func_8003B6AC(0);
        box = (u8 *)TextBox_Create(0, value & 0xFFF, 0x10, 0xB0, 0x120, 0x30);
        DuelEffect_MarkObjectIfActive(box);
        *(u16 *)(box + 0x34) = *(u16 *)(box + 0x34) | 8;
        if ((value & 0x8000) != 0) {
            D_8009B27C |= 0x4000;
            *(u16 *)(box + 0x34) = *(u16 *)(box + 0x34) & 0xFFF7;
        }
        D_8009B28C = D_8009B27C;
        return;
    }
    if ((D_8009B2A4 & 0x4000) != 0) {
        return;
    }
    if ((D_8009B27C & 0x4000) == 0) {
        TextBox_Destroy(D_800EB0F8);
    }
    D_8009B28C = 0;
    D_8009B27C = 0;
}
```
