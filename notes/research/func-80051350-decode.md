# Decode of `func_80051350`

No candidate source yet. This records the structural decode so the first
reconstruction is written from a map rather than from 446 instructions of
assembly. Nothing here is measured against a build.

Takes three arguments and spills all of them immediately to the incoming
argument slots, then reloads them repeatedly; the second argument alone is
re-read at least six times. That spill-and-reload pattern is the shape to
reproduce, not an artefact. The frame is 0x90 bytes and the function saves ten
callee-saved registers, s0 through s7 plus fp and ra, so the arguments lose
theirs.

This file lives under `notes/research/` rather than `notes/candidates/`
deliberately. `notes/candidates/rules.md` requires an entry there to "record the
profile and the measured result in the heading" and to store the exact source
that produces it, and `check-metadata` enforces that by refusing any candidate
note without a recognised compiler profile. A decode with no build has neither,
so it is research, not a candidate. It moves to `notes/candidates/` when there
is a source and a measured figure to put in the heading.

## What it calls and touches

| symbol | how |
| --- | --- |
| `rcos`, `rsin` | once each, both on `D_8009B47A + 0x800` |
| `SquareRoot0` | three times |
| `Model_UpdateViewMetrics` | once |
| `D_800F2C40` | `%hi`/`%lo`, so **not** small data |
| `D_800F56F0` | `%hi`/`%lo` |
| `D_8009AF98`, `D_8009AF99` | `%gp_rel` bytes |
| `D_8009B47A` | `%gp_rel` halfword |

`D_800F2C40` is the `0xE20`-stride record array that `func_8004CB0C.c` and
`func_8004D75C.c` already declare. The offsets here confirm the stride and say
which records are used: `0xDC8` and `0xDCE` are record 0, `0x1BEE` is record 1
because `0x1BEE - 0xDCE = 0xE20`. Reuse that declaration rather than inventing
a new one.

## The opening is one idiom repeated eight times

```
lhu  $v0, OFF($s0)
sll  $v0, $v0, 16 ; sra $v1, $v0, 16      sign-extend the halfword
srl  $v0, $v0, 31 ; addu $v1, $v1, $v0 ; sra $v1, $v1, 1
slt  $v0, $v1, $t3
beqz $v0, .L
addu $v1, $t3, $zero
```

The `srl 31` / `addu` / `sra 1` triple is signed division by two, so this is

```c
v = (s16)rec->field / 2;
if (v < arg1) {
    v = arg1;
}
```

which is a clamp upward against the second argument, run over eight different
halfword fields and stored into consecutive stack slots from `0x10(sp)` up.
Writing it as `max()` or with the divide spelled `>> 1` will not reproduce the
three-instruction division, so keep the `/ 2` on a signed type.

## The clamps are pairs assigned as structs, not scalars

The eight clamps are four *pairs*, and each pair is built in two stack slots and
then copied down eight bytes:

```
clamp(rec0+0xDCE) -> 0x18(sp) ; clamp(rec1+0x1BEE) -> 0x1C(sp)
lw 0x18 ; lw 0x1C ; sw 0x10 ; sw 0x14
clamp(rec0+0xDC8) -> 0x20(sp) ; clamp(rec1+0x1BE8) -> 0x24(sp)
lw 0x20 ; lw 0x24 ; sw 0x18 ; sw 0x1C
clamp(rec0+0xDCA) -> 0x28(sp) ; clamp(rec1+0x1BEA) -> 0x2C(sp)
lw 0x28 ; lw 0x2C ; sw 0x20 ; sw 0x24
```

Build-in-a-temp-then-copy is GCC's shape for a **whole-structure assignment**,
not for two scalar stores.

**Verified with a probe rather than asserted.** Two ten-line functions differing
only in whether the pair is built in a named temporary and assigned, or written
field by field into the array element:

```
struct assignment          scalar stores
sw $4,48($sp)              sw $2,16($sp)
sw $2,52($sp)              sw $4,16($sp)
lw $5,48($sp)              sw $2,20($sp)
lw $6,52($sp)              sw $4,20($sp)
sw $5,16($sp)
sw $6,20($sp)
```

The struct form is **exactly four instructions longer** and produces the
`sw`/`sw` then `lw`/`lw`/`sw`/`sw` sequence the target has at every pair. The
scalar form produces no copy at all.

**That probe proves sufficiency, not necessity, and the conclusion drawn from
it was too strong.** Reading further into the function shows the destinations
are indexed later as `sp + fp*4 + 0x10`, `+0x18`, `+0x20` and so on, so they are
eight two-element `s32` arrays selected by an entity index, not structures. And
the function commits ten callee-saved registers and spills all three arguments,
so it is under heavy register pressure - which produces the identical
compute-into-a-slot-then-copy shape when GCC spills a value and reloads it to
store.

Both explanations emit `sw`/`sw`/`lw`/`lw`/`sw`/`sw`. The probe only shows that
a struct assignment is *one* way to get it. The right statement is that the
shape means **the two values were materialised somewhere before being stored to
their destination**, and the source-level cause has to be decided from the
surrounding code - here the later indexing argues for plain arrays plus
spilling.

**The field order is 3, 0, 1, 2, not 0, 1, 2, 3.** The first pair read is
`0xDCE`/`0x1BEE`, then `0xDC8`, `0xDCA`, `0xDCC` - and `0xDCE` is the *last* of
the four consecutive halfwords `0xDC8`, `0xDCA`, `0xDCC`, `0xDCE`. Reading the
last element first and then the first three is the signature of a polygon or
edge walk that needs the previous vertex before the loop proper, so the source
almost certainly handles element 3 outside the sequence rather than iterating
0 to 3.

Both records are indexed the same way, `rec0` at `0xDC8` and `rec1` at
`0x1BE8`, one `0xE20` stride apart, so the pair is one field from each record.

## Two traps mean a division by a variable

```
bnez $s5, .L800517EC ; break 7
bne  $s5, $at, .L80051804 ; bne $v0, $at, .L80051804 ; break 6
```

`break 7` is MIPS divide-by-zero and `break 6` is the `INT_MIN / -1` overflow
check, so GCC emitted a full checked `div` against `$s5`. That is an ordinary
`/` in the source on a value the compiler cannot prove non-zero - it is not an
SDK helper and should not be written as one.

## Order of work for the first draft

1. The eight clamps, which are a third of the body and entirely mechanical.
2. `rcos`/`rsin` on `D_8009B47A + 0x800`, each multiplied by argument two with
   the `bgez` / `addiu 0xFFF` / `sra 12` rounding idiom - that is a signed
   divide by `0x1000`, so write it as `/ 4096` on a signed value.
3. The `SquareRoot0` block and the divisions.
4. `Model_UpdateViewMetrics` and the tail.

The `lui`-count check should be run as soon as the first draft compiles: with
`D_800F2C40` and `D_800F56F0` both addressed `%hi`/`%lo` while three other
globals are `%gp_rel`, this function is a good candidate for the small-data
mistake that cost `func_80045514` 48 instructions.
