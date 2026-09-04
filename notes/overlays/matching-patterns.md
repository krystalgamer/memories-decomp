# Overlay Matching Patterns

Source-shape rules recovered while matching overlay functions with
`gcc_2_8_1_g0_split`. Each one is stated as the observable difference in the
target, followed by the source construct that reproduces it, and each is
backed by a function that now matches exactly.

These are about recovering what the original author wrote. They are not
tricks for defeating the optimiser: where the difference cannot be expressed
in ordinary C, the function stays as assembly.

## A store fills a load delay slot

If the target issues a global load and fills its delay slot with an unrelated
store, the source read that global into a local **before** the store.

GCC will not hoist a load of a global across a store through a pointer
parameter, because it cannot prove the two do not alias. So it only appears
above the store if it was already above it in the source.

```c
/* one instruction too long: the load lands after the store, with a nop */
*(s16 *)(a + 0x1A) = 0x63;
*(s16 *)(a + 0x18) = D_8016D428 * 16 + 0xA3;

/* matches */
s32 value = D_8016D428;
*(s16 *)(a + 0x1A) = 0x63;
*(s16 *)(a + 0x18) = value * 16 + 0xA3;
```

Verified by `func_8016A00C` in the password module.

## A stall the compiler would have filled

The converse also happens, and it is a stopping condition rather than a
technique. Where two accesses share a base pointer at different offsets, GCC
can prove independence and will reorder freely to fill a delay slot. If the
target leaves the stall in place, no ordinary source ordering reproduces it.

`func_8016A02C` in the password module is the recorded example: the target
keeps a `nop` after `lhu $v1, 0x8($v0)` where GCC hoists the load above the
neighbouring `sb`. It remains assembly rather than being forced.

## Index loops, not pointer walks

Write the loop over an index and let GCC strength-reduce it. Pre-optimising
into a pointer walk folds the array base and the starting offset into a
single `addiu` and comes out an instruction short of the target, which forms
the base and the offset separately.

```c
/* matches: counter set first, base and offset formed separately */
for (i = 15; i >= 0; i--) {
    if (gFreeDuel_apSparklePool[i] == 0) {
        return &gFreeDuel_apSparklePool[i];
    }
}
```

Verified by `FreeDuel_GetSparkleSlot` in the Free Duel module.

## A redundant register copy means a separate variable

A copy that looks pointless usually means the source used its own local for
an intermediate rather than assigning back in place. The extra variable also
changes what the scheduler puts in the delay slot.

```c
/* one instruction short: result goes straight back into the loop variable */
value = value / 10;

/* matches: quotient gets its own local, and the copy frees the delay slot */
next = value / 10;
value = next;
```

Verified by `func_80181EEC` in the main menu module.

The rule is about matching the original's locals, not about avoiding locals.
Read the target both ways:

- If the target does **not** materialise something in a register, do not give
  it a local. `func_80169D10` writes `D_800EB0F8` base-relative at absolute
  offsets, and adding a local for the element makes GCC fold the base and the
  offset into one `addiu`. `FreeDuel_Entry` is the same in miniature: hoisting
  a repeated expression into a local displaces an address load out of a branch
  delay slot.
- If the target **does** materialise one, the local is required.
  `func_801681E8` forms the `D_800F2848` base in its own register and stores
  at constant offsets from it; without a local for that base, GCC walks the
  address incrementally instead.

Both of those are the same question asked in opposite directions: does a
register in the target hold a value the source named?

## Signed and unsigned short tests

A zero test compiled as `sll $rX, $rX, 16` followed by a branch is a **signed**
short; `andi $rX, $rX, 0xffff` is an unsigned one. When the field is loaded
with `lhu` but tested with `sll`, the field is `u16` and the variable holding
it is `s16`.

Observed in `FreeDuel_UpdateSparkle`, where this closed the semantic gap
though that function does not yet match in full.

## Known unresolved residual

`FreeDuel_PlaceCursor` and `FreeDuel_UpdateSparkle` each reduce to a single
transposition of two independent loads, with every other instruction and the
total size already agreeing. Source statement order does not influence it.

Profiles measured against `FreeDuel_UpdateSparkle`, none matching:

| Profile | Result |
|---|---|
| `gcc_2_8_1_g0_split` | correct size, single transposition |
| `gcc_2_8_1_g0` | further |
| `gcc_2_8_1_g0_no_split` | further |
| `gcc_2_8_1_g0_no_sched2` | further |
| `gcc_2_7_2_g0` | furthest, `0xC4` instead of `0xC8` |

The 2.7.2 result is useful in its own right: it is positive evidence that
these modules belong to the GCC 2.8.1 cohort, reached through the recorded
escalation path rather than by assumption.

Two independent functions sharing one residual suggests a scheduler ordering
difference rather than two unrelated source mistakes. Treat it as a single
open question about the profile set instead of guessing per function.

## Statement splitting controls evaluation order

Where the target evaluates the left side of a binary operator before the
right, the source used two statements. C leaves the order unspecified and GCC
will otherwise load the operand first.

```c
/* loads the digit first, and fills the branch delay slot with the address */
packed = (packed << 4) | gPassword_abDigits[i];

/* matches: shift first, address formed inside the loop, delay slot empty */
packed <<= 4;
packed |= gPassword_abDigits[i];
```

## A rotated loop means the source did not use while

`while (cond) { ... }` lets GCC peel the first test and rotate the loop, which
moves the condition to the bottom and changes which branch is inverted. A
target that tests at the top and returns to it with an unconditional `j` was
written as an endless loop with explicit exits.

```c
/* matches: test at top, j back-edge, increment in its delay slot */
for (;;) {
    if (entry[0] == -1) {
        return 0;
    }
    if (packed == entry[1]) {
        return index;
    }
    entry += 2;
    index++;
}
```

Both verified by `func_8016A304` in the password module.
