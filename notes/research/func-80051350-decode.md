# Decode of `func_80051350`

A measured candidate now lives in
[`src/candidates/func_80051350.c`](../../src/candidates/func_80051350.c),
with its
[exact target](../../src/candidates_target/func_80051350.S) and reproducible
fingerprints in
[`candidates.json`](../../config/slus_01411/candidates.json). This note
preserves the structural decode that preceded that candidate; current build
metrics and residual analysis belong in the candidate source comment.

Takes three arguments and spills all of them immediately to the incoming
argument slots, then reloads them repeatedly; the second argument alone is
re-read at least six times. That spill-and-reload pattern is the shape to
reproduce, not an artefact. The frame is 0x90 bytes and the function saves ten
callee-saved registers, s0 through s7 plus fp and ra, so the arguments lose
theirs.

This file remains under `notes/research/` as the pre-candidate structural
record, not as a second candidate source. The build-integrated source, target,
profile, and fingerprint metadata are the current candidate artifacts.

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

`D_800F2C40` is the `0xE20`-stride record array already declared by the
[`func_8004CB0C` candidate](../../src/candidates/func_8004CB0C.c) and matching
`func_8004D75C` in
[`model_slot_row_tables.c`](../../src/game/model_slot_row_tables.c). The
offsets here confirm the stride and say which records are used: `0xDC8` and
`0xDCE` are record 0, `0x1BEE` is record 1 because
`0x1BEE - 0xDCE = 0xE20`. Reuse that declaration rather than inventing a new
one.

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

## What the function is for

The three layers together read as a **separation step**: two records are pushed
apart when they are closer than their combined half-extents.

1. Eight clamps produce, for each of the two records, four half-extent values -
   each a halfword field divided by two and clamped upward against argument two.
   So argument two is a minimum extent.
2. Three delta pairs subtract each record's position fields from a reference
   point taken from `D_800F56F0`, with the first and third components offset by
   the `rcos` and `rsin` of `D_8009B47A + 0x800` scaled by argument two. That is
   a heading-relative offset applied to the reference point.
3. Two `SquareRoot0` calls turn the x and z components of those deltas into a
   distance per record.

Then, per record selected by the index in `$fp`:

```
v0 = (limit - dist) << 12
a1 = v0 / limit                 (checked div - hence break 7 and break 6)
s7 = (delta_x * a1) / 4096      negated when a guard value is positive
s6 = (delta_z * a1) / 4096      likewise
```

`(limit - dist) / limit` in 12-bit fixed point is the fraction of the overlap,
and multiplying the delta by it and shifting back by 12 gives the push-out
vector. The `bgez` / `addiu 0xFFF` / `sra 12` triple around each multiply is the
signed divide by 4096 that the fixed-point convention needs, so those must be
written as `/ 4096` on a signed value rather than `>> 12`.

The guarded `negu` after each is a sign flip chosen by a separate stored value,
which is what makes the push symmetric: one record moves one way and the other
the opposite way.

**The division is by the distance limit, which the compiler cannot prove
non-zero**, which is exactly why `break 7` and `break 6` are present. A
reconstruction that guards the divisor itself, or that uses a helper, will not
emit them.

## The stack layout, and the guard that gates the push

The eight destinations are eight `s32 x[2]` arrays, indexed later by the record
selector in `$fp`. Their final contents, with the clamp order corrected for the
copy-down:

| slot | holds |
| --- | --- |
| `0x10` | clamp of field `0xDCE` |
| `0x18` | clamp of field `0xDC8` |
| `0x20` | clamp of field `0xDCA` |
| `0x28` | clamp of field `0xDCC` |
| `0x30` | delta from the reference x, offset by the `rcos` term |
| `0x38` | delta from the reference y |
| `0x40` | delta from the reference z, offset by the `rsin` term |
| `0x48` | the `SquareRoot0` distance |

Reading the indexed section with those names, the per-record body is

```c
limit = a3[i];
if (limit < a1[i]) limit = a1[i];
if (limit < a0[i]) limit = a0[i];          /* max of three extents */

dy = d1[i];
if (dy < 0) dy = -dy;
if (a2[i] < dy) goto next;                 /* outside the height band */

dist = d[i];
if (dist < 0) goto next;
if (dist >= limit) goto next;              /* no overlap */
```

so the push only happens when the horizontal distance is inside the largest of
three extents **and** the vertical delta is within a fourth. That is why there
are four clamped extents rather than one: three feed a maximum and the fourth is
a separate height test.

Two further gates follow before the division - one on a value the loop clears at
entry and one on the first argument - so the first argument is a mode flag
rather than data.

## The whole thing is a two-iteration loop, and it recurses

The body from `0x80051744` to `0x800519B0` is a loop: `$fp` starts at zero, the
tail does `addiu $fp,$fp,1` and `slti $v0,$fp,0x2` and branches back. So every
indexed array is walked for record 0 then record 1.

The epilogue then does something the rest of the decode did not predict:

```
lw   $t2, 0x98(sp)          the third argument
slti $v0, $t2, 0x3
beqz $v0, .L80051A14
lw   $a1, 0x94(sp)
jal  func_80051350          <- itself
addu $a2, $t2, $zero
```

**The function is recursive, and its third argument is a depth counter capped at
three.** The counter is incremented once in the loop preheader, not per
iteration, and the recursion is additionally gated on the first argument and on
a separate hit counter kept at `0x58(sp)`. So the shape is

```c
s32 func_80051350(s32 mode, s32 min_extent, s32 depth)
{
    s32 moved = 0;        /* $s1, and the return value */
    s32 hits = 0;         /* 0x58(sp) */

    depth = depth + 1;
    for (i = 0; i < 2; i++) {
        ...
    }
    if (mode != 0 && moved != 0) {
        Model_UpdateViewMetrics(0);
    }
    if (hits != 0 && mode != 0 && depth < 3) {
        func_80051350(mode, min_extent, depth);
    }
    return moved;
}
```

That changes the reading of the first argument too: it gates both the
`Model_UpdateViewMetrics` call and the recursion, so it is a "may act" flag
rather than a selector.

The push itself is applied at `0x80051974` as `D_800F56F0[0] += s7` and
`rec->+0x8 += s6`, which is the resolve step; `$s1` is set from the limit when a
record is skipped, so the return value reports the largest extent considered.

**Still undecoded:** the vector block between `0x80051868` and `0x80051930`. It
multiplies pairs drawn from `D_800F56F0` at `+0x8`, `+0xC` and `+0x14`, squares
two differences, feeds a third `SquareRoot0`, and divides by its result under
the same checked-division traps. It also maintains `D_8009AF98` as a countdown
from `0x1E` and `D_8009AF99` as a sign, which look like a shake or recoil timer
rather than part of the separation.

One packing detail to settle before writing that part: the record's word at
`+0xDC0` is read whole and masked with `0xFFFFFF`, while the byte at `+0xDC3` is
read separately with `lbu`. On little-endian that byte is the top of the same
word, so the source has both a word and a byte view of it - writing `w >> 24`
would produce a shift where the target has a load.

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
