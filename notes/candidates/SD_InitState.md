## `SD_InitState` at 0x80046768

`gcc_2_8_1_g8_split`, 125 of 125 instructions, opcode distance 0, positional
diff 8. Ties on `gcc_2_8_1_g8_split_no_strength_reduce` and
`gcc_2_8_1_cc_g8_as_g4_split`.

A real `make match` narrows the residual to a **single byte**:

```
error: mismatch at file offset 0x36fca, VRAM 0x800467ca:
       expected 0x4b, got 0x4a
```

That byte is the `rt` field of `addiu $t3, $v0, %lo(D_80010784)`. The target
allocates the struct-copy temporaries as `$t0`/`$t1`/`$t2` for data with `$t3`
as the source pointer; this build shifts them down one to `$a3`/`$t0`/`$t1`
with `$t2` as the pointer. `$a3` is genuinely free in both builds -- the
function takes one argument and `$a1`/`$a2` die with the zero loop -- so this
is the allocator's preference order, not liveness. Everything else in the
function is byte-identical.

## Three findings that got it here

**The three name copies are struct assignments, not `strcpy`.** `strcpy`
emits a real `jal` because the profiles carry `-fno-builtin`; a struct
assignment expands inline as a block move regardless. The unaligned
`lwl`/`lwr` + `swl`/`swr` shape comes from the *destination* offset being odd
(`0x1619`), not from the source. Each copy needs its own struct size, because
the lengths are the source strings' lengths plus terminator (11, 10, 10).

**The base is an address constant, not a symbol.** Splat prints
`%hi(loaded_image_end)` and `%lo(D_801E0004)`, but a linked executable has no
relocations -- those pairs are splat's inference. Writing the base as
`(u8 *)0x801E0000` lets GCC CSE one `lui` into the base, the `0x801EA7FF`
limit and `g_SDValue`, which is what the target does. Using the symbol forces
the address to be completed and costs a second `lui`/`ori`.

**`D_8009B0F0` and `D_8009B120` need `section(".data")`.** The target reaches
both through the assembler `$at` macro form; declared plainly they go
`$gp`-relative under `-G8` and each loses its `lui`. GCC warns "section
attribute ignored for uninitialized variable" for these, but the warning is
misleading -- removing the attributes measurably regresses the result, so they
are load-bearing here.

## Crossed without improving on the single register shift

Register pressure: extra unused parameters (2, 3 and 4 -- these generate no
code at all, so the idea cannot work in principle), `arg0` held in a local
across the copies, reversed local declaration order, and an early-return guard
keeping the loop cursor live. All byte-identical to the base; probe integrity
was verified with a forced-codegen mutant, so these are real canonicalisations
rather than a stale probe.

Struct-copy spelling (direct, `const`-cast source, block-scoped cursor), all
six source orderings of the three copies (ascending is strictly best), the
five declaration forms of the three source structs (plain, `const`, `.data`,
`.rodata`, `const` plus `.data` -- all identical), shared typedef sizes (does
not type-check, and forcing it would change the copy length), loop form
(`do`/`while`, `do`-pre, `while` -- inert), loop cursor type, a local for one
destination group, and all 29 profiles. GCC 2.7.2 is worse at 124 instructions
and distance 1.

## What is left

Only the `$a3`-versus-`$t0` temp preference. It is fixed before any of the
source-level knobs above take effect. Verify any future attempt with a real
`make match` rather than an object probe: an encoding-distance metric moves by
only one or two for a whole-register shift and cannot rank it reliably.

```c
#include "../../src/types.h"
#include "../../src/game/sound.h"

typedef struct { char c[11]; } SDName11;
typedef struct { char c[10]; } SDName10;

extern SDName11 D_80010784;
extern SDName10 D_80010790;
extern SDName10 D_8001079C;
extern void *D_8009B0F0 __attribute__((section(".data")));
extern void *D_8009B120 __attribute__((section(".data")));
extern void *D_8009B460;

extern void func_8004666C(void);
extern void func_800466C8(void);
extern void func_800478EC(void);
extern void func_800494F4(void *);
extern void func_8004671C(void);
extern void func_80044D48(void);
extern void func_80048F14(void);
extern void SD_SetOutputType(s32);

#define S ((u8 *)g_SDValue)

void SD_InitState(u8 arg0)
{
    u32 *p;
    SDValue *s;
    SDSecondaryState *d;

    g_SDValue = (SDValue *)(u8 *)0x801E0000;
    D_8009B460 = (void *)0x801E1650;
    p = (u32 *)(u8 *)0x801E0000;
    do {
        *p = 0;
        p++;
    } while ((u32)p <= 0x801EA7FF);
    D_8009B0F0 = (void *)func_8004666C;
    D_8009B120 = (void *)func_800466C8;

    *(SDName11 *)(S + 0x1619) = D_80010784;
    *(SDName10 *)(S + 0x1629) = D_80010790;
    *(SDName10 *)(S + 0x1639) = D_8001079C;

    *(S + 0x4A) = 3;
    if (arg0) {
        *(S + 0x4A) |= 0xF0;
    }

    *(S + 0x1649) = 0xFF;
    *(S + 0x164A) = 0xD2;
    *(S + 0x164B) = 0xFF;

    *(S + 0x49) = 0xFF;
    *(s16 *)(S + 0x42) = *(S + 0x1649);
    *(s16 *)(S + 0x44) = *(S + 0x164A);
    *(S + 0x1584) = 0xFF;

    func_800494F4((void *)0x801E1670);
    func_8004671C();
    func_80044D48();
    func_80048F14();
    SD_SetOutputType(0);
    *((u8 *)D_8009B458 + 0x509) = 0;
    d = D_8009B458;
    s = g_SDValue;
    *(void **)((u8 *)d + 0x50C) = (void *)func_800478EC;
    *(s16 *)((u8 *)s + 0x40) = 0;
}
```
