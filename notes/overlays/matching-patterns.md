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

## An offset inside the relocation means the symbol was indexed directly

When the two directions above are hard to call, the relocation itself
usually settles it. Reaching a member through the symbol lets GCC fold the
member offset into the relocation, and every other member is then reached
relative to that fold:

```c
/* does not match */
*(u16 *)(*(u8 **)(D_801845C0 + 4)) = *(u16 *)(D_801845C0 + 0);
```

```
lui   $3, %hi(D_801845C0+4)     # offset folded into the relocation
lw    $4, %lo(D_801845C0+4)($3)
addiu $2, $3, %lo(D_801845C0+4)
lhu   $3, -4($2)                # and the base is now reached backwards
```

A local pins the address so each member stays an ordinary load offset, which
is what the target does:

```c
u8 *state = D_801845C0;
*(u16 *)(*(u8 **)(state + 4)) = *(u16 *)state;
```

Two signals, either of which is enough:

- `%hi(sym+N)` in the build where the target has a plain `%hi(sym)`. This is
  not a relocation quirk to normalise away; it is the source having indexed
  the symbol.
- A **negative** load offset. A struct member should never need one, so it
  means the base register points at some other member.

Verified by `func_80181E30` in the main menu module.

## Signed and unsigned short tests

A zero test compiled as `sll $rX, $rX, 16` followed by a branch is a **signed**
short; `andi $rX, $rX, 0xffff` is an unsigned one. When the field is loaded
with `lhu` but tested with `sll`, the field is `u16` and the variable holding
it is `s16`.

Observed in `FreeDuel_UpdateSparkle`, where this closed the semantic gap
though that function does not yet match in full.

## A flat comparison chain is if/else, and the last arm is the else

GCC 2.8.1 lowers even a small `switch` to a **balanced comparison tree**: it
tests the midpoint with `slt` and halves the range. So a target that compares
against its constants **in a flat sequence**, one `beq` after another, was not
a `switch` in the source no matter how switch-shaped the logic reads.

Given a flat chain, the block layout then says which arm was written last:

- Each test branching **forward** to a block that the chain also reaches by
  **fall-through** means that block is the source's final `else`.
- So if two values share an arm and that shared arm is the fall-through
  target, the source tested the **negated** condition:
  `if (x != a && x != b) { rest } else { shared }`, not
  `if (x == a || x == b) { shared } else { rest }`.

Writing it the natural way round emits the shared arm first and reaches it
with an extra jump, which is the whole difference.

Verified by `func_80184344` in the main menu module, where the type values
`0x14` and `0x17` share a palette. The same function also shows the narrower
tell that a value shifted with `srl` rather than `sra` was held in an
**unsigned** local.

## Two loop invariants: the second one emitted becomes the pointer

When a loop indexes a two-dimensional array with one invariant index and one
running index, as in `table[slot][i]`, GCC hoists two values into the loop
preheader: the address of `table`, and the row offset `slot * sizeof(row)`.
It then strength-reduces exactly one of them into the induction variable and
adds the other on every iteration, so the loop body keeps a visible
`addu $rDest, $rInvariant, $rInduction`.

Which one becomes the induction variable follows the order they are emitted:
the invariant emitted **second** is the one that increments. This is
observable without a full match, because the two setup values also land in
swapped registers.

```
sll   $t0, $v0, 3           # row offset emitted first  -> stays invariant
lui   $v0, %hi(table)
addiu $a0, $v0, %lo(table)  # table address emitted second -> increments
.Lloop:
addu  $v1, $t0, $a0
...
addiu $a0, $a0, 4
```

Writing the row offset as a statement **before** the loop does move it ahead
of the address, but it also lets GCC prove `table + offset` invariant, so the
two fold into a single pointer before the loop and the per-iteration `addu`
disappears. That is the same folding described under the pointer-walk rule
above.

Computing the offset **inside the loop body** resolves both. GCC then creates
the induction variable itself rather than seeing a variable the source
already placed, so the offset stays a separate integer, and its initialiser
is emitted in the loop preheader instead of ahead of it:

```c
for (i = 0; i < count; i++) {
    offset = i * stride + base_offset;  /* body: GCC creates the giv */
    entry = record + offset;
```

Measured on `func_80168050`, shared by both overworld overlays, where this
reproduces the target exactly. The same source with `offset` initialised
before the loop, or folded into `record + i * stride + base_offset`, fails in
the two ways described above.

The mirror case is when the **address** is the one that must increment, so the
loop body reads `addu $rDest, $rOffset, $rTable` with `addiu $rTable, $rTable,
stride` at the bottom. Letting GCC create that giv folds the row offset into
its initial value, because both halves are invariant. Walking an explicit
source pointer instead keeps them apart:

```c
offset = slot * row_bytes;
p = table;
while (i < count) {
    entry = ...p and offset...;
    p++;
    i++;
}
```

Here `p` varies, so `p + offset` is not invariant and survives into the body.
Measured on `func_801840F8` in the main menu module. Initialising the counter
**before** `offset` and `p` also matters: with the counter left to a `for`
initialiser it is emitted after them and the two loop registers come out
swapped.

## Which operand comes first in a two-register add

`addu $rD, $rA, $rB` and `addu $rD, $rB, $rA` are different words, so operand
order is part of the match. Two rules decide it.

**Integer addition keeps source order; pointer addition does not.** The C front
end rewrites `int + pointer` back to `pointer + int`, so a target that adds an
invariant offset *before* a pointer cannot be written as `offset + p`. Making
the addition integer-typed preserves the order:

```c
entry = (Type *)(offset + (s32)p);   /* addu $rD, $rOffset, $rP */
entry = (Type *)((u8 *)p + offset);  /* addu $rD, $rP, $rOffset */
```

**Accumulating into a variable pins it to the first operand.** `x += v` emits
`addu $rX, $rX, $rV`, because the destination is also the left operand. When
the target reads `addu $rX, $rV, $rX` the sum went into a **different**
variable, and the operands then follow source order:

```c
total = entry->count + amount;       /* addu $rTotal, $rCount, $rAmount */
```

This is the same evidence as the locals rule — a register holding a value the
source named — read through the operand fields rather than through an extra
copy.

Both measured on `func_801840F8` in the main menu module.

## Emission order distinguishes hand-written from generated code

The preheader gives a reliable reading of which values the source named.
Ordinary assignments are emitted where the source puts them; loop-invariant
addresses and induction-variable initialisers are emitted in the preheader,
after them. So in

```
jal   <call>
...   record         # before the hoisted addresses -> written by hand
addu  $s4, $zero, $zero   # loop counter -> written by hand
lui   ...            # hoisted address
lui   ...            # hoisted address
addiu $s2, $zero, 0x12    # after them -> compiler-generated giv
```

`record` and the counter must be source variables assigned after the call,
and the offset must not be. Reading the order this way pinned every saved
register in `func_80168050` before the first full build.

The corollary is that a value emitted *before* a hoisted address cannot be
made to move after it by reordering statements, because no source position
maps there. When a setup value sits in the wrong place relative to the
hoisted addresses, the fix is to stop naming it, not to move it.

Measured on `func_801840F8` in `main_menu`, where `table[slot][i]` reproduces
the instruction count exactly, including the `361 << 3` expansion of the
`2888`-byte row stride, and on `func_80168050` in the overworld overlays,
where the body-computed offset produces a full match.

## Known unresolved residual

`FreeDuel_PlaceCursor` and `FreeDuel_UpdateSparkle` each reduce to a single
transposition of two independent loads, with every other instruction and the
total size already agreeing. Source statement order does not influence it.

Re-measured on `FreeDuel_UpdateSparkle` in more detail, the residual is
narrower than "two loads" suggests, and the extra detail rules things out.
The function contains two independent chains, and **both** are transposed as
a unit — not just their loads:

```
target   lbu $v0, 0xC   lhu $v1, 0x60   addiu $v0,-4   addiu $v1,-1
built    lhu $v1, 0x60  lbu $v0, 0xC    addiu $v1,-1   addiu $v0,-4
```

Every other instruction, including the deferred `sb` in the branch delay
slot, is identical, and so is the **register allocation**: the level chain
gets `$v0` and the timer chain `$v1` in both. That last point matters,
because it means there is no allocation component to work with. The liveness
lever that resolved `func_80180F50` — giving values separate locals so one
stays live longer — cannot apply, since allocation is already correct and
only the emission order of two equal-priority chains differs.

Swapping the two source statements was measured directly and produces
byte-identical output, so the order is chosen by the scheduler rather than
inherited from the source.

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
